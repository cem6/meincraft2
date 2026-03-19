#include "renderer.h"

GLuint VAO, VBO, shaderProgram;
GLint modelLoc, viewLoc, projLoc, texLoc;

void renderer_init() {
	shaderProgram = shader_create("res/shaders/cube.vert", "res/shaders/cube.frag");
	texture_create("res/images/purple.png");

	modelLoc = glGetUniformLocation(shaderProgram, "model");
	viewLoc  = glGetUniformLocation(shaderProgram, "view");
	projLoc  = glGetUniformLocation(shaderProgram, "projection");
	texLoc   = glGetUniformLocation(shaderProgram, "ourTexture");

	// float vertices[] = {
	// 	// Positions          // Colors (R, G, B)
	// 	// Back face (Red)
	// 	-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
	// 	0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
	// 	0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
	// 	0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
	// 	-0.5f,  0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
	// 	-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,
	//
	// 	// Front face (Green)
	// 	-0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
	// 	0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
	// 	0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
	// 	0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
	// 	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
	// 	-0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,
	//
	// 	// Left face (Blue)
	// 	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
	// 	-0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
	// 	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
	// 	-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, 1.0f,
	// 	-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
	// 	-0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,
	//
	// 	// Right face (Yellow)
	// 	0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
	// 	0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
	// 	0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
	// 	0.5f, -0.5f, -0.5f,  1.0f, 1.0f, 0.0f,
	// 	0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
	// 	0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.0f,
	//
	// 	// Bottom face (Magenta)
	// 	-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
	// 	0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
	// 	0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
	// 	0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
	// 	-0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 1.0f,
	// 	-0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 1.0f,
	//
	// 	// Top face (Cyan)
	// 	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
	// 	0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f,
	// 	0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
	// 	0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
	// 	-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 1.0f,
	// 	-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, 1.0f
	// };

	float vertices[] = {
		// Positions          // Texture Coords (U, V)
		// Back face
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		// Front face
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		// Left face
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		// Right face
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		// Bottom face
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		// Top face
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};

	// vao
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// vbo
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// color
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void renderer_frame() {
	glUseProgram(shaderProgram);

	// texture
	glActiveTexture(GL_TEXTURE0);	
	glBindTexture(GL_TEXTURE_2D, texture);
	// send to shader
	glUniform1i(texLoc, 0);

	// math
	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)WINDOW_WIDTH / WINDOW_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = glm::lookAt(camera.pos, camera.pos + camera.front, camera.up);

  	// float time = SDL_GetTicks() / 1000.0f;
   	// glm::mat4 model = glm::rotate(glm::mat4(1.0f), time * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));
	glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f));

	// send to shader
 	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}

void renderer_destroy() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);
}
