#include "text.h"
#include "ui.h"

std::unordered_map<char, Character> Characters;

GLuint vao, vbo;
GLuint textShaderProgram;

void text_init() {
	FT_Library ft;
	if (FT_Init_FreeType(&ft)) {
		std::cerr << "Error: freetype init" << std::endl;
		return;
	}

	FT_Face face;
	if (FT_New_Face(ft, "res/fonts/Arial.ttf", 0, &face)) {
		std::cerr << "Error: freetype load face" << std::endl;
		return;
	}

	FT_Set_Pixel_Sizes(face, 0, 48);

	// generate glyphs
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // disable byte alignment restriction
	for (unsigned char c = 0; c < 128; c++) {
		// load glyph
		if (FT_Load_Char(face, c, FT_LOAD_RENDER)) {
			std::cout << "Error: freetype failed to load glyph" << std::endl;
			continue;
		}
		// generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 
			   						0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// store
		Character ch = {texture, 
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters[c] = ch;
	}
	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	textShaderProgram = shader_create("res/shaders/text.vert", "res/shaders/text.frag");


	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void text_draw(std::string text, float x, float y, float scale, glm::vec4 color) {
	glDisable(GL_DEPTH_TEST);

	glUseProgram(textShaderProgram);

	glm::mat4 projection = glm::ortho(0.0f, (float)WINDOW_WIDTH, 0.0f, (float)WINDOW_HEIGHT);
	glUniformMatrix4fv(glGetUniformLocation(textShaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

	glUniform3f(glGetUniformLocation(textShaderProgram, "textColor"), color.x, color.y, color.z);

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(vao);

	// iterate all chars
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++) {
		Character ch = Characters[*c];

		float xpos = x + ch.bearing.x * scale;
		float ypos = y - (ch.size.y - ch.bearing.y) * scale;

		float w = ch.size.x * scale;
		float h = ch.size.y * scale;

		float vertices[6][4] = {
			{ xpos,     ypos + h,   0.0f, 0.0f },            
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },

			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }           
		};

		glBindTexture(GL_TEXTURE_2D, ch.textureID);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		x += (ch.advance >> 6) * scale;
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glEnable(GL_DEPTH_TEST);
}

float _calc_string_width(std::string text, float scale) {
	float width = 0.0f;
	for (char c : text) {
		Character ch = Characters[c];
		width += (ch.advance >> 6) * scale;
	}
	return width;
}

void text_draw_box(std::string text, float x, float y, float scale, glm::vec4 textcolor, glm::vec4 boxcolor) {
	float padding = 10.0f;
	float width = _calc_string_width(text, scale);
	float height = 48.0f * scale;

	ui_draw_quad(x - padding, y - padding, width + 2*padding, height + 2*padding, boxcolor);
	text_draw(text, x, y, scale, textcolor);
}
