#include "ui.h"
#include "text.h"

GLuint uiShaderProgram;
// GLuint aspectLoc;

GLuint qvao, qvbo;

void _quad_init() {
	float vertices[] = {
		0.0f, 1.0f,   0.0f, 1.0f,
		0.0f, 0.0f,   0.0f, 0.0f,
		1.0f, 0.0f,   1.0f, 0.0f,

		0.0f, 1.0f,   0.0f, 1.0f,
		1.0f, 0.0f,   1.0f, 0.0f,
		1.0f, 1.0f,   1.0f, 1.0f
	};

	glGenVertexArrays(1, &qvao);
    glGenBuffers(1, &qvbo);

    glBindVertexArray(qvao);
    glBindBuffer(GL_ARRAY_BUFFER, qvbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

void ui_draw_quad(float x, float y, float width, float height, glm::vec4 color) {
	glUseProgram(uiShaderProgram);
	glDisable(GL_DEPTH_TEST);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(x, y, 0.0f));
	model = glm::scale(model, glm::vec3(width, height, 1.0f));

	glUniformMatrix4fv(glGetUniformLocation(uiShaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniform4f(glGetUniformLocation(uiShaderProgram, "color"), color.r, color.g, color.b, color.a);

	glBindVertexArray(qvao);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
	glEnable(GL_DEPTH_TEST);
}

void ui_init() {
	uiShaderProgram = shader_create("res/shaders/ui.vert", "res/shaders/ui.frag");
	// aspectLoc = glGetUniformLocation(uiShaderProgram, "aspect");

	// crosshair_init();
	_quad_init();
	text_init();
}

void ui_draw() {
	glDisable(GL_DEPTH_TEST);
	glUseProgram(uiShaderProgram);

	// glUniform1f(aspectLoc, (float)WINDOW_WIDTH / WINDOW_HEIGHT);
	glm::mat4 projection = glm::ortho(0.0f, float(WINDOW_WIDTH), 0.0f, float(WINDOW_HEIGHT));
	glUniformMatrix4fv(glGetUniformLocation(uiShaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	debug_draw();

	// corsshair
	glEnable(GL_COLOR_LOGIC_OP);
	glLogicOp(GL_INVERT);
	
	float centerX = WINDOW_WIDTH / 2.0f;
	float centerY = WINDOW_HEIGHT / 2.0f;
	float size = 16.0f;
	
	ui_draw_quad(centerX - 1, centerY - size/2, 2, size, glm::vec4(1,1,1,1));
	ui_draw_quad(centerX - size/2, centerY - 1, size, 2, glm::vec4(1,1,1,1));

	glDisable(GL_COLOR_LOGIC_OP);
	glEnable(GL_DEPTH_TEST);
}
