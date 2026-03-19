#ifndef WORLD_H
#define WORLD_H

#include "../util/util.h"
#include "chunk.h"

struct World {
	std::unordered_map<glm::ivec3, Chunk*, PosHasher> chunkMap;
};
extern World world;

void world_init();
void world_destroy();
blocktype world_get_block(glm::ivec3);
void world_update_chunks(glm::vec3 cameraPos);

#endif
