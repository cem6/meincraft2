#ifndef WORLD_H
#define WORLD_H

#include "../util/util.h"
#include "chunk.h"
#include "../render/culling.h"

struct World {
	std::unordered_map<glm::ivec3, Chunk*, Vec3Hasher> chunkMap;
	// std::unordered_map<glm::ivec2, int, Vec2Hasher> heightMap;
	std::vector<Chunk*> dirtyChunks;
};
extern World world;

void world_init();
void world_destroy();
blocktype world_get_block(glm::ivec3);
void world_update_chunks(glm::vec3 cameraPos);

bool world_chunk_visible(glm::ivec3);

#endif
