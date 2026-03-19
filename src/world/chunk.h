#ifndef CHUNK_H
#define CHUNK_H

#include "../util/util.h"

enum blocktype {
	AIR,
	STONE,
	DIRT,
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
	glm::ivec3 position;
};

int chunk_pos_to_idx(int x, int y, int z);
void chunk_generate_mesh(Chunk *chunk);
void chunk_init(Chunk *chunk, glm::ivec3 pos);

#endif
