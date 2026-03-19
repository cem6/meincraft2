#include "chunk.h"

int chunk_pos_to_idx(int x, int y, int z) {
	return x * CHUNK_SIZE_X * CHUNK_SIZE_Z + z * CHUNK_SIZE_Z + y;
}

bool _block_is_transparent(Chunk *chunk, int x, int y, int z) {
	// TODO: check neighboring chunks
	if (x < 0 || x >= CHUNK_SIZE_X || 
	 	y < 0 || y >= CHUNK_SIZE_Y || 
	 	z < 0 || z >= CHUNK_SIZE_Z) return true;

	return chunk->blocks[chunk_pos_to_idx(x, y, z)] == AIR;
}

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

void _add_face(std::vector<float> &vertices, int x, int y, int z, face fi) {
	for (int i = 0; i < 6; i++) {
		int offset = i * 5;
		vertices.push_back(face_data[fi][offset + 0] + x);
		vertices.push_back(face_data[fi][offset + 1] + y);
		vertices.push_back(face_data[fi][offset + 2] + z);

		vertices.push_back(face_data[fi][offset + 3]);
		vertices.push_back(face_data[fi][offset + 4]);
	}
}

void chunk_generate_mesh(Chunk *chunk) {
	std::vector<float> vertices; // 	(CHUNK_VOLUME * 15);

	for (int x = 0; x < CHUNK_SIZE_X; x++) {
		for (int y = 0; y < CHUNK_SIZE_Y; y++) {
			for (int z = 0; z < CHUNK_SIZE_Z; z++) {
				blocktype block = chunk->blocks[chunk_pos_to_idx(x, y, z)];
				if (block == AIR) continue;

				// check neighbor is transparent
				if (_block_is_transparent(chunk, x, y + 1, z)) _add_face(vertices, x, y, z, TOP);
				if (_block_is_transparent(chunk, x, y - 1, z)) _add_face(vertices, x, y, z, BOTTOM);
				if (_block_is_transparent(chunk, x - 1, y, z)) _add_face(vertices, x, y, z, LEFT);
				if (_block_is_transparent(chunk, x + 1, y, z)) _add_face(vertices, x, y, z, RIGHT);
				if (_block_is_transparent(chunk, x, y, z + 1)) _add_face(vertices, x, y, z, FRONT);
				if (_block_is_transparent(chunk, x, y, z - 1)) _add_face(vertices, x, y, z, BACK);
			}
		}
	}

	chunk->vertexCount = vertices.size() / 5;
	glBindBuffer(GL_ARRAY_BUFFER, chunk->vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
}

void chunk_init(Chunk *chunk, glm::ivec3 pos) {
	chunk->position = pos;
	// vao
	glGenVertexArrays(1, &chunk->vao);
	glBindVertexArray(chunk->vao);

	// vbo
	glGenBuffers(1, &chunk->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, chunk->vbo);
	// glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// texture
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// unbind
	// glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
} 
