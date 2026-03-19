#include "renderer.h"

GLuint VAO, VBO, shaderProgram;
GLint modelLoc, viewLoc, projLoc, texLoc;

Chunk tChunk;

void renderer_init() {
	shaderProgram = shader_create("res/shaders/cube.vert", "res/shaders/cube.frag");
	texture_create("res/images/purple.png");

	modelLoc = glGetUniformLocation(shaderProgram, "model");
	viewLoc  = glGetUniformLocation(shaderProgram, "view");
	projLoc  = glGetUniformLocation(shaderProgram, "projection");
	texLoc   = glGetUniformLocation(shaderProgram, "ourTexture");

	// TODO: worldgen
	chunk_init(&tChunk, glm::ivec3(0, 0, 0));

	for (int x = 0; x < CHUNK_SIZE_X; x++) {
		for (int y = 0; y < CHUNK_SIZE_Y; y++) {
			for (int z = 0; z < CHUNK_SIZE_Z; z++) {
				tChunk.blocks[chunk_pos_to_idx(x, y, z)] = STONE;
			}
		}
	}

	chunk_generate_mesh(&tChunk);
}

void _draw_chunk(Chunk *chunk) {
	if (chunk->vertexCount == 0) return;

	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(chunk->vao);
	glDrawArrays(GL_TRIANGLES, 0, chunk->vertexCount);
}

void renderer_frame() {
	glUseProgram(shaderProgram);

	// texture
	glActiveTexture(GL_TEXTURE0);	
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(texLoc, 0);

	// math
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = glm::lookAt(camera.pos, camera.pos + camera.front, camera.up);
 	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	// TODO: worldgen
	_draw_chunk(&tChunk);
}

void renderer_destroy() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);
}
