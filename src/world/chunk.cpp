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
		blocktype nb = world_get_block((glm::ivec3){gx, gy, gz});
		return nb == AIR || nb == WATER;
	}
	
	blocktype nb = chunk->blocks[chunk_pos_to_idx(x, y, z)];
	return nb == AIR || nb == WATER;
}

glm::vec2 _get_atlas_coords(blocktype type, face fi) {
	switch (type) {
		case STONE: return glm::vec2(0, 17);
		case DIRT: 	return glm::vec2(1, 17);
		case GRASS: 
			if (fi == TOP) 		return glm::vec2(3, 17);
			if (fi == BOTTOM) 	return glm::vec2(1, 17);
			return glm::vec2(2, 17);
		case SAND: 	return glm::vec2(4, 17);
		case WATER: return glm::vec2(5, 17);

		default: return glm::vec2(15, 0);
	};
}

void _add_face(std::vector<float> &vertices, int x, int y, int z, blocktype block, face fi) {
	glm::vec2 atlasPos = _get_atlas_coords(block, fi);

	const float step = 0.0625f; // 1/16 tiles in atlas
	
	const float atlasSize = 324.0f;
	const float slotSize = 18.0f;
	const float texSize = 16.0f;
	const float gap = 1.0f;
	const float uv_inset = 0.01f;
	
	// water top bisschen runter
	float yOffset = 0;
	if (block == WATER && fi == TOP) {
		yOffset = -0.1f;
	}

	for (int i = 0; i < 6; i++) {
		int offset = i * 8;

		// positions
		vertices.push_back(face_data[fi][offset + 0] + x);
		vertices.push_back(face_data[fi][offset + 1] + y + yOffset);
		vertices.push_back(face_data[fi][offset + 2] + z);


		// textures
		// vertices.push_back((atlasPos.x + face_data[fi][offset + 3]) * step);
		// vertices.push_back((atlasPos.y + face_data[fi][offset + 4]) * step);

		float u = face_data[fi][offset + 3];
		float v = face_data[fi][offset + 4];
		float u_adj = (u == 0.0f) ? (u + uv_inset) : (u - uv_inset);
		float v_adj = (v== 0.0f) ? (v + uv_inset) : (v - uv_inset);
		vertices.push_back(((atlasPos.x * slotSize) + gap + (u_adj * texSize)) / atlasSize);
		vertices.push_back(((atlasPos.y * slotSize) + gap + (v_adj * texSize)) / atlasSize);
	
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

	static std::vector<float> transparentVertices; 
	if (transparentVertices.capacity() == 0) transparentVertices.reserve(CHUNK_VOLUME * 6);
	transparentVertices.clear();

	for (int x = 0; x < CHUNK_SIZE_X; x++) {
		for (int y = 0; y < CHUNK_SIZE_Y; y++) {
			for (int z = 0; z < CHUNK_SIZE_Z; z++) {
				blocktype block = chunk->blocks[chunk_pos_to_idx(x, y, z)];
				if (block == AIR) continue;

				std::vector<float> &target = (block == WATER) ? transparentVertices : vertices;

				// only consider top for water blocks
				if (block == WATER) {
					if (_block_is_transparent(chunk, x, y + 1, z)) _add_face(target, x, y, z, block, TOP);
					continue;
				}

				// check neighbor is transparent
				if (_block_is_transparent(chunk, x, y + 1, z)) _add_face(target, x, y, z, block, TOP);
				if (_block_is_transparent(chunk, x, y - 1, z)) _add_face(target, x, y, z, block, BOTTOM);
				if (_block_is_transparent(chunk, x - 1, y, z)) _add_face(target, x, y, z, block, LEFT);
				if (_block_is_transparent(chunk, x + 1, y, z)) _add_face(target, x, y, z, block, RIGHT);
				if (_block_is_transparent(chunk, x, y, z + 1)) _add_face(target, x, y, z, block, FRONT);
				if (_block_is_transparent(chunk, x, y, z - 1)) _add_face(target, x, y, z, block, BACK);
			}
		}
	}

	chunk->vertexCount = vertices.size() / 8;
	glBindBuffer(GL_ARRAY_BUFFER, chunk->vbo);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

	chunk->transparentVertexCount = transparentVertices.size() / 8;
	glBindBuffer(GL_ARRAY_BUFFER, chunk->transparentVbo);
	glBufferData(GL_ARRAY_BUFFER, transparentVertices.size() * sizeof(float), transparentVertices.data(), GL_STATIC_DRAW);
}

void chunk_init(Chunk *chunk, glm::ivec3 pos) {
	chunk->pos = pos;
	// vao, vbo
	glGenVertexArrays(1, &chunk->vao);
	glBindVertexArray(chunk->vao);
	glGenBuffers(1, &chunk->vbo);
	glBindBuffer(GL_ARRAY_BUFFER, chunk->vbo);

	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// normals
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);


	// transparent vao, vbo
	glGenVertexArrays(1, &chunk->transparentVao);
	glBindVertexArray(chunk->transparentVao);
	glGenBuffers(1, &chunk->transparentVbo);
	glBindBuffer(GL_ARRAY_BUFFER, chunk->transparentVbo);

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
