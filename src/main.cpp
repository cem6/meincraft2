#include "util/util.h"
#include "render/window.h"

using namespace std;

int main() {
	// init sdl 
	if (SDL_Init(SDL_INIT_VIDEO) == false) {
		cout << "Init Error: " << SDL_GetError() << endl;
		return -1;
	}

	// opengl attributes
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	window_create();
	window_loop();
		
	return 0;
}
