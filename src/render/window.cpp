#include "window.h"

SDL_Window* window;
SDL_GLContext context;
SDL_Event event;

void window_create() {
	window = SDL_CreateWindow("test", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_OPENGL);
	context = SDL_GL_CreateContext(window);
}

void _destroy() {
	SDL_GL_DestroyContext(context);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void window_loop() {
	bool running = true;
	while (running) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) running = false;
		}

		glClearColor(0.1f, 0.5f, 0.9f, 1);
		glClear(GL_COLOR_BUFFER_BIT);

		SDL_GL_SwapWindow(window);
	}

	_destroy();
}

