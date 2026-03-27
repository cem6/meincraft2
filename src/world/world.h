#ifndef WORLD_H
#define WORLD_H

#include "../util/util.h"
#include "chunk.h"
#include "../render/culling.h"

struct World {
	std::unordered_map<glm::ivec3, Chunk*, Vec3Hasher> chunkMap; // generated chunks
	// std::unordered_map<glm::ivec2, int, Vec2Hasher> heightMap; // highest y in chunk
	std::vector<glm::ivec3> loadQ; // generate blockdata
	std::vector<Chunk*> dirtyChunks; // generate chunkdata
	std::unordered_set<glm::ivec3, Vec3Hasher> pendingChunks;  // can be an attribute in chunk
};
extern World world;

void world_init();
void world_destroy();
blocktype world_get_block(glm::ivec3);
bool world_chunk_visible(glm::ivec3);

void world_update_load_queue(glm::vec3 cameraPos);
void world_process_blockdata_generation(glm::vec3 cameraPos);
void world_process_mesh_generation(glm::vec3 cameraPos);

#endif
