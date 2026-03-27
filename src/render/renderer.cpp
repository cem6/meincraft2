#include "renderer.h"
#include "../util/debug.h"

GLuint shaderProgram;
GLint modelLoc, viewLoc, projLoc, texLoc, 
	camposLoc, lightposLoc, renderDistLoc;

void renderer_init() {
	// shaderProgram = shader_create("res/shaders/cube.vert", "res/shaders/cube.frag");
	shaderProgram = shader_create("res/shaders/light.vert", "res/shaders/light.frag");
	texture_create("res/images/atlas1.png");
	// texture_create("res/images/test.png");

	modelLoc = glGetUniformLocation(shaderProgram, "model");
	viewLoc  = glGetUniformLocation(shaderProgram, "view");
	projLoc  = glGetUniformLocation(shaderProgram, "projection");
	texLoc   = glGetUniformLocation(shaderProgram, "ourTexture");
	camposLoc   	= glGetUniformLocation(shaderProgram, "camPos");
	lightposLoc   	= glGetUniformLocation(shaderProgram, "lightPos");
	renderDistLoc 	= glGetUniformLocation(shaderProgram, "renderDist");

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

void _draw_chunks(std::vector<Chunk*> &chunks) {
	for (auto &c : chunks) {
		if (c->vertexCount > 0) {
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(
		c->pos.x * CHUNK_SIZE_X,
		c->pos.y * CHUNK_SIZE_Y,
		c->pos.z * CHUNK_SIZE_Z
	));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glBindVertexArray(c->vao);
			glDrawArrays(GL_TRIANGLES, 0, c->vertexCount);
		}
	}

	glDepthMask(GL_FALSE);
	for (auto &c : chunks) {
		if (c->transparentVertexCount > 0) {
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(
		c->pos.x * CHUNK_SIZE_X,
		c->pos.y * CHUNK_SIZE_Y,
		c->pos.z * CHUNK_SIZE_Z
	));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
			glBindVertexArray(c->transparentVao);
			glDrawArrays(GL_TRIANGLES, 0, c->transparentVertexCount);
		}
	}
	glDepthMask(GL_TRUE);
}

void _draw_radius(glm::vec3 cameraPos) {
	int rendered_chunks = 0;
	static std::vector<Chunk*> chunks;
	chunks.clear();

	// camera pos -> chunk pos
	int camX = floor(cameraPos.x / CHUNK_SIZE_X);
	int camY = floor(cameraPos.y / CHUNK_SIZE_Y);
	int camZ = floor(cameraPos.z / CHUNK_SIZE_Z);

	const int maxDistSq = RENDER_DISTANCE * RENDER_DISTANCE;
	
	// schrittweise kreis optimization
	for (int x = -RENDER_DISTANCE; x <= RENDER_DISTANCE; x++) {
	// for (int tx = 0; tx <= 2*RENDER_DISTANCE; tx++) {
	// 	int x = (tx % 2 == 0) ? tx/2 : -( (tx+1)/2 );
		int xSq = x * x;
		for (int z = -RENDER_DISTANCE; z <= RENDER_DISTANCE; z++) {
		// for (int tz = 0; tz <= 2*RENDER_DISTANCE; tz++) {
		// 	int z = (tz % 2 == 0) ? tz/2 : -( (tz+1)/2 );
			int xzSq = xSq + (z * z);

			// x z dist far enough -> skip y
			if (xzSq >= maxDistSq) continue;

			for (int y = -RENDER_DISTANCE; y <= RENDER_DISTANCE; y++) {
				// kompletter kreis
				// if (xzSq + (y * y) > maxDistSq) continue; 
				// kein y limit ?
				glm::ivec3 pos(camX + x, camY + y, camZ + z);

				auto it = world.chunkMap.find(pos);
				// if (it != world.chunkMap.end() && world_chunk_visible(pos)) {
				if (it != world.chunkMap.end() && world_chunk_visible(pos) && culling_chunk_visible(pos)) {
					rendered_chunks++;
					// _draw_chunk(it->second);
					chunks.push_back(it->second);
				}
			}
		}
	}
	
	_draw_chunks(chunks);


	debug.rendered_chunks = rendered_chunks;
}

void renderer_draw_frame() {
	glUseProgram(shaderProgram);

	// light
	glUniform3f(camposLoc, camera.pos.x, camera.pos.y, camera.pos.z);
	glUniform3f(lightposLoc, 0.0f, 200.0f, 0.0f);
	glUniform1f(renderDistLoc, RENDER_DISTANCE);

	// texture
	glActiveTexture(GL_TEXTURE0);	
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(texLoc, 0);

	// camera math
	// 3d camera, fov: 45.0 -> 70, 59 -> 90, always calculate for culling
	glm::mat4 projection = glm::perspective(glm::radians(59.0f), (float)WINDOW_WIDTH / WINDOW_HEIGHT, 0.5f, 800.0f);
	glm::mat4 view = glm::lookAt(camera.pos, camera.pos + camera.front, camera.up);
	if (FRUSTUM_CULLING) culling_update_planes(projection * view);
	// 2d camera
	if (camera.view_2d) {
		const float aspect = (float)WINDOW_WIDTH / WINDOW_HEIGHT;
		const float zoom = (RENDER_DISTANCE * CHUNK_SIZE_X) + 32.0f; // 300.0f;
		projection = glm::ortho(-aspect * zoom, aspect * zoom, -zoom, zoom, 0.1f, 1000.0f);
		view = glm::lookAt(camera.pos + glm::vec3(0, 100, 0), camera.pos, glm::vec3(1, 0, 0));
	}
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	
	static int frameCnt = 0;
	if (frameCnt++ % 20 == 0) world_update_load_queue(camera.pos);

	// world_update_chunks(camera.pos); // limited generations per frame
	world_process_blockdata_generation(camera.pos);
	world_process_mesh_generation(camera.pos);
	_draw_radius(camera.pos);
}

void renderer_destroy() {
	world_destroy();
	glDeleteProgram(shaderProgram);
}
