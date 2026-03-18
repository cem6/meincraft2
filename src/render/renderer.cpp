#include "renderer.h"

GLuint VAO, VBO, shaderProgram;

void renderer_init() {
	shaderProgram = shader_create("res/shaders/triangle.vs", "res/shaders/triangle.fs");

	float vertices[] = {
  		-0.5f, -0.5f, 0.0f, // Left  
         0.5f, -0.5f, 0.0f, // Right 
         0.0f,  0.5f, 0.0f  // Top  
	};

	// vao
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// vbo
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// layout
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void renderer_frame() {
	glUseProgram(shaderProgram);

	glBindVertexArray(VAO);

	glDrawArrays(GL_TRIANGLES, 0, 3);
}

void renderer_destroy() {
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shaderProgram);
}
