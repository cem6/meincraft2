#include "world.h"

World world;

#define TEST_SIZE 10

void _generate_test() {
	// create all block data
	for (int cx = 0; cx < TEST_SIZE; cx++) {
		for (int cz = 0; cz < TEST_SIZE; cz++) {
			Chunk *c = new Chunk();

			chunk_init(c, glm::ivec3(cx, 0, cz));

			for (int x = 0; x < CHUNK_SIZE_X; x++) {
				for (int z = 0; z < CHUNK_SIZE_Z; z++) {
					// TODO: noise
					int height = (int)(sin((cx * CHUNK_SIZE_X + x) * 0.1f) * 3.0f + cos((cz * CHUNK_SIZE_Z + z) * 0.1f) * 3.0f) + 16;

					for (int y = 0; y < CHUNK_SIZE_Y; y++) {
						if (y < height) c->blocks[chunk_pos_to_idx(x, y, z)] = STONE;
						else if (y == height) c->blocks[chunk_pos_to_idx(x, y, z)] = DIRT;
						else c->blocks[chunk_pos_to_idx(x, y, z)] = AIR;
					}

				}
			}

			world.chunkMap[c->pos] = c;
		}
	}

	// create meshes (neighbor blocks have to exist)
	for (auto [_, c] : world.chunkMap) chunk_generate_mesh(c);
}

void world_init() {
	_generate_test();
}

void world_destroy() {
	for (auto [_, c] : world.chunkMap) {
		glDeleteVertexArrays(1, &c->vao);	
		glDeleteBuffers(1, &c->vbo);
		delete c;
	}
	world.chunkMap.clear();
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
