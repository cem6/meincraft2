#include "renderer.h"
#include "../ui/debug.h"

GLuint shaderProgram;
GLint modelLoc, viewLoc, projLoc, texLoc, aimPosLoc, aimNormLoc,
	camposLoc, lightposLoc, timeLoc,
	renderDistLoc, waterLevelLoc;

void renderer_init() {
	shaderProgram = shader_create("res/shaders/world.vert", "res/shaders/world.frag");
	texture_create("res/images/atlas1.png");
	// texture_create("res/images/test.png");
	
	modelLoc = glGetUniformLocation(shaderProgram, "model");
	viewLoc  = glGetUniformLocation(shaderProgram, "view");
	projLoc  = glGetUniformLocation(shaderProgram, "projection");
	texLoc   = glGetUniformLocation(shaderProgram, "ourTexture");
	aimPosLoc    	= glGetUniformLocation(shaderProgram, "aimPos");
	aimNormLoc    	= glGetUniformLocation(shaderProgram, "aimNorm");
	camposLoc   	= glGetUniformLocation(shaderProgram, "camPos");
	lightposLoc   	= glGetUniformLocation(shaderProgram, "lightPos");
	timeLoc   		= glGetUniformLocation(shaderProgram, "time");
	renderDistLoc 	= glGetUniformLocation(shaderProgram, "renderDist");
	waterLevelLoc 	= glGetUniformLocation(shaderProgram, "waterLevel");

	world_init();
	ui_init();
	// text_init();

	glUseProgram(shaderProgram);
	glUniform1i(texLoc, 0);

	glUniform1f(renderDistLoc, RENDER_DISTANCE);
	glUniform1f(waterLevelLoc, WATER_LEVEL);
}

void renderer_destroy() {
	world_destroy();
	glDeleteProgram(shaderProgram);
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
	// draw transparent after others blocks for transparency to consider others
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
				if (it != world.chunkMap.end() && world_chunk_visible(pos) && culling_chunk_visible(pos)) {
					chunks.push_back(it->second);
				}
			}
		}
	}
	_draw_chunks(chunks);

	debug.rendered_chunks = chunks.size();
}

void renderer_draw_frame() {
	glUseProgram(shaderProgram);

	glUniform3f(camposLoc, camera.pos.x, camera.pos.y, camera.pos.z);
	glUniform3f(lightposLoc, 0.0f, 200.0f, 0.0f);
	// glUniform1f(timeLoc, SDL_GetTicks() / 1000.0f);

	// texture
	glActiveTexture(GL_TEXTURE0);	
	glBindTexture(GL_TEXTURE_2D, texture);

	// looking at highlight
	Raycast r = camera_get_raycast(RAYCAST_DIST);
	if (r.hit){
		glUniform3i(aimPosLoc, r.blockPos.x, r.blockPos.y, r.blockPos.z);
		glUniform3i(aimNormLoc, r.normal.x, r.normal.y, r.normal.z);
	} 
	else glUniform3i(aimPosLoc, 0, INT_MIN, 0);

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

	world_process_blockdata_generation(camera.pos);
	world_process_mesh_generation(camera.pos);
	_draw_radius(camera.pos);

	ui_draw();
}
