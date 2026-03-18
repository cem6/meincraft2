#include "window.h"

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
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

	window = SDL_CreateWindow("Voxel", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
	context = SDL_GL_CreateContext(window);

	// SDL_SetWindowMouseGrab(window, true);
	SDL_SetWindowRelativeMouseMode(window, true);

	// init glad
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		std::cerr << "Error: Glad Init failed" << std::endl;
		return;
	}

	renderer_init();
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
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
		float deltaTime = (frameStart = lastFrameTime) / 1000.0f;
		lastFrameTime = frameStart;

		// events
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) running = false;
			if (event.type == SDL_EVENT_KEY_DOWN) {
				if (event.key.key == SDLK_ESCAPE) running = false;
				if (event.key.key == SDLK_B) {
					SDL_SetWindowRelativeMouseMode(window, !SDL_GetWindowRelativeMouseMode(window)); // toggle capture mouse
				}
			}
			// only move camera when mouse captured
			if (event.type == SDL_EVENT_MOUSE_MOTION && SDL_GetWindowRelativeMouseMode(window)) {
				camera_update_rotation(event.motion.xrel, event.motion.yrel);
			}
		}

		camera_update_position(deltaTime);

		// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		// render
		glClearColor(0.1f, 0.1f, 0.1f, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		renderer_frame();
		SDL_GL_SwapWindow(window);

		// wait if finished early
		frameTime = SDL_GetTicks() - frameStart;
		if (FRAME_DELAY > frameTime) SDL_Delay(FRAME_DELAY - frameTime);
	}

	_destroy();
}

