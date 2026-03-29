#include "crosshair.h"

GLuint cvao, cvbo;

void crosshair_init() {
	float size = 0.02f;
	float vertices[] = {
		-size, 0.0f, 0.0f,  size, 0.0f, 0.0f, // horizontal line
		0.0f, -size, 0.0f,  0.0f, size, 0.0f  // vertical line
	};

	glGenVertexArrays(1, &cvao);
	glGenBuffers(1, &cvbo);
	glBindVertexArray(cvao);
	glBindBuffer(GL_ARRAY_BUFFER, cvbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
}

void crosshair_bind_vao() {
	glBindVertexArray(cvao);
}
