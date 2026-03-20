#include "world.h"

#include "noise/FastNoiseLite.h"
#include "../util/debug.h"

World world;
FastNoiseLite noise;

// TODO: move to chunk.
Chunk* _generate_blockdata(glm::ivec3 pos) {
	Chunk *c = new Chunk();
	chunk_init(c, pos);

	int xOffset = pos.x * CHUNK_SIZE_X;
	int yOffset = pos.y * CHUNK_SIZE_Y;
	int zOffset = pos.z * CHUNK_SIZE_Z;

	for (int x = 0; x < CHUNK_SIZE_X; x++) {
		for (int z = 0; z < CHUNK_SIZE_Z; z++) {
			// int height = (int)(sin((xOffset + x) * 0.1f) * 3.0f + cos((zOffset + z) * 0.1f) * 3.0f) + 16;
			int height = (noise.GetNoise(float(xOffset + x), float(zOffset + z)) + 1) * 16 + 16;

			for (int y = 0; y < CHUNK_SIZE_Y; y++) {
				int gy = yOffset + y;

				if (gy < height - 3) 	c->blocks[chunk_pos_to_idx(x, y, z)] = STONE;
				else if (gy == height) 	c->blocks[chunk_pos_to_idx(x, y, z)] = GRASS;
				else if (gy < height) 	c->blocks[chunk_pos_to_idx(x, y, z)] = DIRT;
				else c->blocks[chunk_pos_to_idx(x, y, z)] = AIR;
			}

		}
	}

	return world.chunkMap[c->pos] = c;
}

void _generate_initial_world() {
	// create all block data
	for (int cy = 0; cy < RENDER_DISTANCE/2; cy++) {
		for (int cx = -RENDER_DISTANCE; cx < RENDER_DISTANCE; cx++) {
			for (int cz = -RENDER_DISTANCE; cz < RENDER_DISTANCE; cz++) {
				Chunk *c = new Chunk();
				chunk_init(c, glm::ivec3(cx, cy, cz));
				_generate_blockdata(glm::ivec3(cx, cy, cz));
			}
		}
	}

	// create meshes (neighbor blocks have to exist)
	for (auto &[_, c] : world.chunkMap) chunk_generate_mesh(c);
}

void world_init() {
	noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2S);
	_generate_initial_world();
}

void world_destroy() {
	for (auto &[_, c] : world.chunkMap) {
		glDeleteVertexArrays(1, &c->vao);	
		glDeleteBuffers(1, &c->vbo);
		delete c;
	}
	world.chunkMap.clear();
	world.dirtyChunks.clear();
}

blocktype world_get_block(glm::ivec3 globalPos) {
	// chunk position
    glm::ivec3 cp (
		(globalPos.x >= 0) ? (globalPos.x / CHUNK_SIZE_X) : ((globalPos.x - CHUNK_SIZE_X + 1) / CHUNK_SIZE_X),
		(globalPos.y >= 0) ? (globalPos.y / CHUNK_SIZE_Y) : ((globalPos.y - CHUNK_SIZE_Y + 1) / CHUNK_SIZE_Y),
		(globalPos.z >= 0) ? (globalPos.z / CHUNK_SIZE_Z) : ((globalPos.z - CHUNK_SIZE_Z + 1) / CHUNK_SIZE_Z)
	);

	// block position in chunk
    glm::ivec3 lp (
		((globalPos.x % CHUNK_SIZE_X) + CHUNK_SIZE_X) % CHUNK_SIZE_X,
		((globalPos.y % CHUNK_SIZE_Y) + CHUNK_SIZE_Y) % CHUNK_SIZE_Y,
		((globalPos.z % CHUNK_SIZE_Z) + CHUNK_SIZE_Z) % CHUNK_SIZE_Z
	);

    if (world.chunkMap.count(cp))
        return world.chunkMap[cp]->blocks[chunk_pos_to_idx(lp)];

    return AIR;
}

glm::ivec3 neighbors[6] = {
	{1.0, 0.0, 0.0},
	{-1.0, 0.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, -1.0, 0.0},
	{0.0, 0.0, 1.0},
	{0.0, 0.0, -1.0}
};
void world_update_chunks(glm::vec3 cameraPos) {
	int camX = floor(cameraPos.x / CHUNK_SIZE_X);
	int camY = floor(cameraPos.y / CHUNK_SIZE_Y);
	int camZ = floor(cameraPos.z / CHUNK_SIZE_Z);

	// per frame
	int blockdata_generated = 0;

	// check: all blockdata in radius exists
	for (int x = -RENDER_DISTANCE; x <= RENDER_DISTANCE; x++) {
		for (int z = -RENDER_DISTANCE; z <= RENDER_DISTANCE; z++) {

			int height = (noise.GetNoise(float(camX * CHUNK_SIZE_X + x), float(camZ * CHUNK_SIZE_Z + z)) + 1) * 16 + 16;

			for (int y = RENDER_DISTANCE; y >= -RENDER_DISTANCE; y--) { // falschrum -> oben zuerst
				if (blockdata_generated >= MAX_GENERATIONS_PER_FRAME) break;

				// dont generate sky chunks
				if ((camY + y - 1) * CHUNK_SIZE_Y > height) continue;
				// dont generate zu weit unten
				if ((camY + y) * CHUNK_SIZE_Y < -CHUNK_SIZE_Y * 3) continue;

				glm::ivec3 pos(camX + x, camY + y, camZ + z);
				if (!world.chunkMap.count(pos)) { 
					Chunk *c = _generate_blockdata(pos);

					// int neiSolidCnt = 0;
					for (int i = 0; i < 6; i++) {
						glm::ivec3 neiPos = pos + neighbors[i];
						// mark neighbors need mesh update
						if (world.chunkMap.count(neiPos)) {
							world.dirtyChunks.push_back(world.chunkMap[neiPos]);
							// if (world.chunkMap[neiPos]->solid) neiSolidCnt++;
						}
					}

					// if (c->solid && neiSolidCnt == 6) c->hidden = true;

					blockdata_generated++;	
				}
			}
		}
	}
	// std::cout << "dirty chunks " << world.dirtyChunks.size() << std::endl;
	debug.dirty_chunks = world.dirtyChunks.size();

	// weiter oben zuerst
	std::sort(world.dirtyChunks.begin(), world.dirtyChunks.end(), [cameraPos](Chunk *a, Chunk *b) {
		return a->pos.y > b->pos.y;
	});

	// regenereate mesh for chunks (TODO not all)
	int mesh_generated = 0;
	for (Chunk *c : world.dirtyChunks) {
		if (mesh_generated == 2*MAX_GENERATIONS_PER_FRAME) break;
		chunk_generate_mesh(c);
		mesh_generated++;
	}
	// world.dirtyChunks.clear();
	world.dirtyChunks.erase(world.dirtyChunks.begin(), world.dirtyChunks.begin() + mesh_generated);

	debug.generated_chunks = world.chunkMap.size();
}
