#ifndef UI_H
#define UI_H

#include "../util/util.h"

#include "../render/shader.h"

#include "debug.h"

void ui_init();
void ui_draw();

void ui_draw_quad(float x, float y, float widht, float height, glm::vec4 color);

#endif
