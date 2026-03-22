#include "chunk.h"

#include "world.h" // ??? für world_get_block

int chunk_pos_to_idx(int x, int y, int z) {
	return x * CHUNK_SIZE_X * CHUNK_SIZE_Z + z * CHUNK_SIZE_Z + y;
	// return y * CHUNK_SIZE_X * CHUNK_SIZE_Z + z * CHUNK_SIZE_X + x;
}
int chunk_pos_to_idx(glm::ivec3 p) {
	return chunk_pos_to_idx(p.x, p.y, p.z);
}

bool _block_is_transparent(Chunk *chunk, int x, int y, int z) {
	// not in this chunk
	if (x < 0 || x >= CHUNK_SIZE_X || 
	 	y < 0 || y >= CHUNK_SIZE_Y || 
	 	z < 0 || z >= CHUNK_SIZE_Z) {

		// relative (in chunk) pos -> global pos
		int gx = chunk->pos.x * CHUNK_SIZE_X + x;
		int gy = chunk->pos.y * CHUNK_SIZE_Y + y;
		int gz = chunk->pos.z * CHUNK_SIZE_Z + z;
		return world_get_block((glm::ivec3){gx, gy, gz}) == AIR;
	}

	return chunk->blocks[chunk_pos_to_idx(x, y, z)] == AIR;
}

glm::vec2 _get_atlas_coords(blocktype type, face fi) {
	switch (type) {
		case STONE: return glm::vec2(0, 15);
		case DIRT: 	return glm::vec2(1, 15);
		case SAND: 	return glm::vec2(4, 14);
		case GRASS: 
			if (fi == TOP) 		return glm::vec2(3, 15);
			if (fi == BOTTOM) 	return glm::vec2(1, 15);
			return glm::vec2(2, 15);

		default: return glm::vec2(15, 0);
	};
}

void _add_face(std::vector<float> &vertices, int x, int y, int z, blocktype block, face fi) {
	glm::vec2 atlasPos = _get_atlas_coords(block, fi);
	const float step = 0.0625f; // 1/16 tiles in atlas
	// const float step = 1;

	for (int i = 0; i < 6; i++) {
		int offset = i * 8;

		// positions
		vertices.push_back(face_data[fi][offset + 0] + x);
		vertices.push_back(face_data[fi][offset + 1] + y);
		vertices.push_back(face_data[fi][offset + 2] + z);

		// textures
		vertices.push_back((atlasPos.x + face_data[fi][offset + 3]) * step);
		vertices.push_back((atlasPos.y + face_data[fi][offset + 4]) * step);
	
		// normals
		vertices.push_back((face_data[fi][offset + 5]));
		vertices.push_back((face_data[fi][offset + 6]));
		vertices.push_back((face_data[fi][offset + 7]));
	}
}

void chunk_generate_mesh(Chunk *chunk) {
	// static -> reuse for all chunks
	static std::vector<float> vertices; 
	if (vertices.capacity() == 0) vertices.reserve(CHUNK_VOLUME * 6);
	vertices.clear();

	for (int x = 0; x < CHUNK_SIZE_X; x++) {
		for (int y = 0; y < CHUNK_SIZE_Y; y++) {
			for (int z = 0; z < CHUNK_SIZE_Z; z++) {
				blocktype block = chunk->blocks[chunk_pos_to_idx(x, y, z)];
				if (block == AIR) continue;

				// check neighbor is transparent
				if (_block_is_transparent(chunk, x, y + 1, z)) _add_face(vertices, x, y, z, block, TOP);
				if (_block_is_transparent(chunk, x, y - 1, z)) _add_face(vertices, x, y, z, block, BOTTOM);
				if (_block_is_transparent(chunk, x - 1, y, z)) _add_face(vertices, x, y, z, block, LEFT);
				if (_block_is_transparent(chunk, x + 1, y, z)) _add_face(vertices, x, y, z, block, RIGHT);
				if (_block_is_transparent(chunk, x, y, z + 1)) _add_face(vertices, x, y, z, block, FRONT);
				if (_block_is_transparent(chunk, x, y, z - 1)) _add_face(vertices, x, y, z, block, BACK);
			}
		}
	}

	chunk->vertexCount = vertices.size() / 5;
	glBindBuffer(GL_ARRAY_BUFFER, chunk->vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
}

void chunk_init(Chunk *chunk, glm::ivec3 pos) {
	chunk->pos = pos;
	// vao
	glGenVertexArrays(1, &chunk->vao);
	glBindVertexArray(chunk->vao);

	// vbo
	glGenBuffers(1, &chunk->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, chunk->vbo);
	// glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// texture
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// normals
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// unbind
	// glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
} 
