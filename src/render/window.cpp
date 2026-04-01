#include "window.h"

#include "../ui/debug.h"

SDL_Window* window = nullptr;
SDL_GLContext context = nullptr;

void window_create() {
	// init sdl 
	if (SDL_Init(SDL_INIT_VIDEO) == false) {
		std::cerr << "Error: Sdl Init failed: " << SDL_GetError() << std::endl;
		return;
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	// SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	// SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

	window = SDL_CreateWindow("Voxel", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
	context = SDL_GL_CreateContext(window);

	SDL_SetWindowRelativeMouseMode(window, true);

	// init glad
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		std::cerr << "Error: Glad Init failed" << std::endl;
		return;
	}

	renderer_init();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// glEnable(GL_MULTISAMPLE);
	//
	// glEnable(GL_CULL_FACE);
	// glCullFace(GL_BACK);
	// glFrontFace(GL_CCW);

	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
}

void _destroy() {
	renderer_destroy();

	SDL_GL_DestroyContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void window_loop() {
	Uint64 frameStart;
	Uint64 lastFrameTime = SDL_GetTicks();
	int frameTime;

	bool running = true;
	SDL_Event event;

	while (running) {
		frameStart = SDL_GetTicks();
		float deltaTime = (frameStart - lastFrameTime) / 1000.0f;
		lastFrameTime = frameStart;

		// events
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) running = false;
			// keyboard
			if (event.type == SDL_EVENT_KEY_DOWN) {
				if (event.key.key == SDLK_ESCAPE) running = false;
				// toggle capture mouse
				if (event.key.key == SDLK_B) SDL_SetWindowRelativeMouseMode(window, !SDL_GetWindowRelativeMouseMode(window)); 
				// set lines
				if (event.key.key == SDLK_E) {
					int polygonMode[2];
					glGetIntegerv(GL_POLYGON_MODE, polygonMode);
					if (polygonMode[0] == GL_LINE) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
					else glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
				}
				// toggle camera mode
				if (event.key.key == SDLK_C) camera.toggle_view();
				// toggle player physics
				if (event.key.key == SDLK_F) camera.toggle_physics();
				// toggle debug text visible
				if (event.key.key == SDLK_TAB) debug.toggle_visible();
			}
			// mouse buttons
			// TODO: get raycast somewhere else
			if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
				if (event.button.button == SDL_BUTTON_RIGHT) player_try_place_block();
				if (event.button.button == SDL_BUTTON_LEFT) player_try_break_block();
			}
			if (event.type == SDL_EVENT_MOUSE_WHEEL) {
				if (event.wheel.y > 0) player_next_block();
				if (event.wheel.y < 0) player_prev_block();
			}
			// only move camera when mouse captured 
			if (event.type == SDL_EVENT_MOUSE_MOTION && SDL_GetWindowRelativeMouseMode(window)) {
				camera_update_rotation(event.motion.xrel, event.motion.yrel);
			}
		}

		camera_update_position(deltaTime);

		// render
		glClearColor(0.4f, 0.6f, 1.0f, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		renderer_draw_frame();

		SDL_GL_SwapWindow(window);

		// wait if finished early
		frameTime = SDL_GetTicks() - frameStart;
		static int c = 0;
		if (c++ % 5 == 0) debug.frame_ms = frameTime;
		if (FRAME_DELAY > frameTime) SDL_Delay(FRAME_DELAY - frameTime);
	}

	_destroy();
}

