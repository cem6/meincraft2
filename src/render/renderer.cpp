#include "renderer.h"

GLuint shaderProgram;
GLint modelLoc, viewLoc, projLoc, texLoc;

void renderer_init() {
	shaderProgram = shader_create("res/shaders/cube.vert", "res/shaders/cube.frag");
	texture_create("res/images/atlas.png");
	// texture_create("res/images/test.png");

	modelLoc = glGetUniformLocation(shaderProgram, "model");
	viewLoc  = glGetUniformLocation(shaderProgram, "view");
	projLoc  = glGetUniformLocation(shaderProgram, "projection");
	texLoc   = glGetUniformLocation(shaderProgram, "ourTexture");

	world_init();
}

void _draw_chunk(Chunk *chunk) {
	if (chunk->vertexCount == 0) return;

	// chunk pos zu global pos
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(
		chunk->pos.x * CHUNK_SIZE_X,
		chunk->pos.y * CHUNK_SIZE_Y,
		chunk->pos.z * CHUNK_SIZE_Z
	));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(chunk->vao);
	glDrawArrays(GL_TRIANGLES, 0, chunk->vertexCount);
}

void _draw_radius(glm::vec3 cameraPos) {
	// camera pos -> chunk pos
	int camX = floor(cameraPos.x / CHUNK_SIZE_X);
	int camY = floor(cameraPos.y / CHUNK_SIZE_Y);
	int camZ = floor(cameraPos.z / CHUNK_SIZE_Z);

	for (int x = -RENDER_DISTANCE; x <= RENDER_DISTANCE; x++) {
		for (int y = -RENDER_DISTANCE; y <= RENDER_DISTANCE; y++) {
			for (int z = -RENDER_DISTANCE; z <= RENDER_DISTANCE; z++) {
				glm::ivec3 pos(camX + x, camY + y, camZ + z);
				// float distSq = (x*x) + (y*y) + (z*z);
				// if (distSq > RENDER_DISTANCE*RENDER_DISTANCE) continue;
				if (world.chunkMap.count(pos)) _draw_chunk(world.chunkMap[pos]);
			}
		}
	}
	//
}

void renderer_draw_frame() {
	glUseProgram(shaderProgram);

	// texture
	glActiveTexture(GL_TEXTURE0);	
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(texLoc, 0);

	// math
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / WINDOW_HEIGHT, 0.5f, 800.0f);
	glm::mat4 view = glm::lookAt(camera.pos, camera.pos + camera.front, camera.up);
 	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

	// for (auto &[_, c] : world.chunkMap) _draw_chunk(c);
	_draw_radius(camera.pos);
}

void renderer_destroy() {
	world_destroy();
	glDeleteProgram(shaderProgram);
}
