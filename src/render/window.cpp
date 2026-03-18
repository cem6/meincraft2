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

	window = SDL_CreateWindow("Voxel", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
	context = SDL_GL_CreateContext(window);

	// init glad
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		std::cerr << "Error: Glad Init failed" << std::endl;
		return;
	}

	renderer_init();
	glEnable(GL_DEPTH_TEST);
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
}

void _destroy() {
	renderer_destroy();

	SDL_GL_DestroyContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void window_loop() {
	bool running = true;
	SDL_Event event;

	while (running) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) running = false;
			if (event.type == SDL_EVENT_KEY_DOWN) {
				if (event.key.key == SDLK_ESCAPE) running = false;
			}
		}

		glClearColor(0.1f, 0.5f, 0.9f, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderer_frame();

		SDL_GL_SwapWindow(window);
	}

	_destroy();
}

