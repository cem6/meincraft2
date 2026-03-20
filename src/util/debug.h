#ifndef DEBUG_H
#define DEBUG_H

#include "util.h"

#include "imgui.h"
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_opengl3.h"

extern struct Debug {
	int rendered_chunks;
	int generated_chunks;
	int dirty_chunks;
	int frame_ms;
} debug;

void debug_init(SDL_Window*, SDL_GLContext);
void debug_pass_event(SDL_Event&);
void debug_create_window();
void debug_render();

#endif
