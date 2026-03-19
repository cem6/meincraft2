#ifndef RENDERER_H
#define RENDERER_H

#include "../util/util.h"
#include "shader.h"
#include "camera.h"
#include "texture.h"
#include "../world/chunk.h"

void renderer_init();
void renderer_frame();
void renderer_destroy();

#endif
