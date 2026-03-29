#ifndef TEXT_H
#define TEXT_H

#include "../util/util.h"
#include "../render/shader.h"


#include <ft2build.h>
#include FT_FREETYPE_H

struct Character {
	GLuint textureID; 	// id of glyph texture
	glm::ivec2 size;	// size of glyph
	glm::ivec2 bearing; // offset of baseline to top left of glyph
	long advance;		// offset to next glyph
};

void text_init();
void text_draw(std::string text, float x, float y, float scale, glm::vec4 color);
void text_draw_box(std::string text, float x, float y, float scale, glm::vec4 textcolor, glm::vec4 boxcolor);

#endif
