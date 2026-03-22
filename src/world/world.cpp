#include "world.h"

#include "noise/FastNoiseLite.h"
#include "../util/debug.h"

#include <chrono>
std::chrono::steady_clock::time_point startTime;
bool isMeasuring = false;
int max_dc = 0;

World world;
FastNoiseLite noise;

glm::ivec3 neighbors[6] = {
	{1.0, 0.0, 0.0},
	{-1.0, 0.0, 0.0},
	{0.0, 1.0, 0.0},
	{0.0, -1.0, 0.0},
	{0.0, 0.0, 1.0},
	{0.0, 0.0, -1.0}
};

float _get_noise_height(float x, float z) {
    float total = 0;
    float frequency = 0.5f;  // How "stretched" the noise is
    float amplitude = 1.2f;  // How "tall" the noise is
    float maxValue = 0;      // Used for normalizing
    int octaves = 8;         // How many layers of detail

    for(int i = 0; i < octaves; i++) {
        total += noise.GetNoise(x * frequency, z * frequency) * amplitude;
        
        maxValue += amplitude;
        amplitude *= 0.5f;   // Each layer is half as tall (Persistence)
        frequency *= 2.0f;   // Each layer is twice as detailed (Lacunarity)
    }

	total = pow(total, 2);
    return (total / maxValue + 1) * CHUNK_SIZE_X + 16;
}

// TODO: move to chunk.
Chunk* _generate_blockdata(glm::ivec3 pos) {
	Chunk *c = new Chunk();
	chunk_init(c, pos);

	int xOffset = pos.x * CHUNK_SIZE_X;
	int yOffset = pos.y * CHUNK_SIZE_Y;
	int zOffset = pos.z * CHUNK_SIZE_Z;

	c->solid = true;
	c->dirty = false;

	for (int x = 0; x < CHUNK_SIZE_X; x++) {
		for (int z = 0; z < CHUNK_SIZE_Z; z++) {
			// int height = (noise.GetNoise(float(xOffset + x), float(zOffset + z)) + 1) * 32 + 16;
			int height = _get_noise_height(xOffset + x, zOffset + z);

			for (int y = 0; y < CHUNK_SIZE_Y; y++) {
				int gy = yOffset + y;

				// ??????????????????????
				if (gy > 52 && gy <= height) c->blocks[chunk_pos_to_idx(x, y, z)] = SAND;
				else if (gy < height - 3) 	c->blocks[chunk_pos_to_idx(x, y, z)] = STONE;
				else if (gy == height) 		c->blocks[chunk_pos_to_idx(x, y, z)] = GRASS;
				else if (gy < height) 		c->blocks[chunk_pos_to_idx(x, y, z)] = DIRT;
				else {
					c->solid = false;
					c->blocks[chunk_pos_to_idx(x, y, z)] = AIR;
				}
			}

		}
	}

	return world.chunkMap[c->pos] = c;
}

void _generate_initial_world() {
	// create all block data
	for (int cy = 0; cy < RENDER_DISTANCE/4; cy++) {
		for (int cx = -RENDER_DISTANCE/2; cx < RENDER_DISTANCE/2; cx++) {
			for (int cz = -RENDER_DISTANCE/2; cz < RENDER_DISTANCE/2; cz++) {
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

// TODO: move somwhere else (to chunk ???)
bool world_chunk_visible(glm::ivec3 pos) {
    if (!world.chunkMap[pos]->solid) return true; // not solid -> has air -> not hidden

    for (int i = 0; i < 6; i++) {
        glm::ivec3 neiPos = pos + neighbors[i];
        
		// on border -> visible
        if (world.chunkMap.find(neiPos) == world.chunkMap.end()) return true;
		// has neighbor that isnt solid -> visible
        if (!world.chunkMap[neiPos]->solid) return true;
    }

    return false; // 6 solid neighbors -> hidden
}

void world_update_chunks(glm::vec3 cameraPos) {
	// render time test
	if (!world.dirtyChunks.empty() && !isMeasuring) {
		startTime = std::chrono::steady_clock::now();
		isMeasuring = true;
	}

	int camX = floor(cameraPos.x / CHUNK_SIZE_X);
	int camY = floor(cameraPos.y / CHUNK_SIZE_Y);
	int camZ = floor(cameraPos.z / CHUNK_SIZE_Z);

	// --- generate blockdata
	int blockdata_generated = 0;

	// spiral pattern -> closer chunks first (sozusagen)
	for (int d = 0; d <= RENDER_DISTANCE; d++) {
		for (int x = -d; x <= d; x++) { 
		for (int z = -d; z <= d; z++) {
			if (std::abs(x) != d && std::abs(z) != d) continue; // only do outer "spiral"

			// int height = (noise.GetNoise(float(camX * CHUNK_SIZE_X + x), float(camZ * CHUNK_SIZE_Z + z)) + 1) * 32 + 16;
			int height = _get_noise_height(camX * CHUNK_SIZE_X + x, camZ * CHUNK_SIZE_Z + z);

			for (int y = RENDER_DISTANCE; y >= -RENDER_DISTANCE; y--) { // falschrum -> oben zuerst
				if (blockdata_generated >= MAX_BLOCKDATA_GENERATIONS_PER_FRAME) break;

				// dont generate sky chunks
				if ((camY + y - 1) * CHUNK_SIZE_Y > height) continue;
				// dont generate zu weit unten
				if ((camY + y) * CHUNK_SIZE_Y < MIN_Y) continue;

				glm::ivec3 pos(camX + x, camY + y, camZ + z);
				if (!world.chunkMap.count(pos)) { 
					Chunk *c = _generate_blockdata(pos);

					// iterate over neighbor chunks
					for (int i = 0; i < 6; i++) {
						glm::ivec3 neiPos = pos + neighbors[i];
						auto it = world.chunkMap.find(neiPos);
						if (it != world.chunkMap.end()) {
							Chunk *nc = it->second;
							// mark neighbors need mesh update
							if (!nc->dirty) {
								nc->dirty = true;
								world.dirtyChunks.push_back(world.chunkMap[neiPos]);
							}
						}
					}

					blockdata_generated++;	
				}
			}
		}
		}
	}

	// --- generate mesh

	// std::cout << "dirty chunks " << world.dirtyChunks.size() << std::endl;
	debug.dirty_chunks = world.dirtyChunks.size();
	max_dc = std::max(max_dc, (int)world.dirtyChunks.size());

	// sort -> generate closer chunks first
	
	// std::sort(world.dirtyChunks.begin(), world.dirtyChunks.end(), [&cameraPos](Chunk *a, Chunk *b) {
	// 	glm::ivec3 c = glm::ivec3(floor(cameraPos.x / CHUNK_SIZE_X), 
	// 						floor(cameraPos.y / CHUNK_SIZE_Y), 
	// 						floor(cameraPos.z / CHUNK_SIZE_Z));
	// 	int distA = std::abs(a->pos.x - c.x) + std::abs(a->pos.y - c.y) + std::abs(a->pos.z - c.z);
	// 	int distB = std::abs(b->pos.x - c.x) + std::abs(b->pos.y - c.y) + std::abs(b->pos.z - c.z);
	// 	return distA < distB;
	// });

	// bisschen besser als sort
	int limit = std::min((int)world.dirtyChunks.size(), MAX_MESH_GENERATIONS_PER_FRAME);
	std::nth_element(world.dirtyChunks.begin(), world.dirtyChunks.begin() + limit, world.dirtyChunks.end(), [&cameraPos](Chunk *a, Chunk *b) {
		glm::ivec3 c = glm::ivec3(floor(cameraPos.x / CHUNK_SIZE_X), 
							floor(cameraPos.y / CHUNK_SIZE_Y), 
							floor(cameraPos.z / CHUNK_SIZE_Z));
		int distA = std::abs(a->pos.x - c.x) + std::abs(a->pos.y - c.y) + std::abs(a->pos.z - c.z);
		int distB = std::abs(b->pos.x - c.x) + std::abs(b->pos.y - c.y) + std::abs(b->pos.z - c.z);
		return distA < distB;
	});

	// regenereate mesh for chunks
	int mesh_generated = 0;
	for (Chunk *c : world.dirtyChunks) {
		if (mesh_generated == MAX_MESH_GENERATIONS_PER_FRAME) break;
		chunk_generate_mesh(c);
		c->dirty = false;
		mesh_generated++;
	}
	// world.dirtyChunks.clear();
	world.dirtyChunks.erase(world.dirtyChunks.begin(), world.dirtyChunks.begin() + mesh_generated);

	debug.generated_chunks = world.chunkMap.size();

	// render time test
	if (isMeasuring && world.dirtyChunks.empty()) {
		auto endTime = std::chrono::steady_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

		std::cout << "ms: " << duration.count() << ", max dirty chunks: " << max_dc << std::endl;
		isMeasuring = false;
	}
}
