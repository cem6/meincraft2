#ifndef CHUNK_H
#define CHUNK_H

#include "../util/util.h"

enum blocktype {
	AIR,
	STONE,
	DIRT,
	GRASS,
	SAND,
};

enum face {
	TOP,
	BOTTOM,
	LEFT,
	RIGHT,
	FRONT,
	BACK,
};

struct Chunk {
	blocktype blocks[CHUNK_VOLUME];
	GLuint vao, vbo;
	int vertexCount;
	glm::ivec3 pos;

	bool dirty; // check duplicate before adding to dirty list
	bool solid; // dont render when solid (no air) + all neighbors solid
};

struct Vec3Hasher {
	std::size_t operator()(const glm::ivec3 &v) const {
		std::size_t h = 0;
		auto hash_int = std::hash<int>{};

		h ^= hash_int(v.x) + 0x9e3779b9 + (h << 6) + (h >> 2);
		h ^= hash_int(v.y) + 0x9e3779b9 + (h << 6) + (h >> 2);
		h ^= hash_int(v.z) + 0x9e3779b9 + (h << 6) + (h >> 2);

		return h;
	}
};

struct Vec2Hasher {
	std::size_t operator()(const glm::ivec2 &v) const {
		std::size_t h = 0;
		auto hash_int = std::hash<int>{};

		h ^= hash_int(v.x) + 0x9e3779b9 + (h << 6) + (h >> 2);
		h ^= hash_int(v.y) + 0x9e3779b9 + (h << 6) + (h >> 2);

		return h;
	}
};

int chunk_pos_to_idx(int x, int y, int z);
int chunk_pos_to_idx(glm::ivec3);

void chunk_generate_mesh(Chunk *chunk);
void chunk_init(Chunk *chunk, glm::ivec3 pos);

const float face_data[6][30] = {
	// top
	{	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	},
	// bottom
	{	-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
	},
	// left
	{	-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	},
	// right
	{	0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
	},
	// front
	{	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
	},
	// back
	{	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
	},
};

#endif
