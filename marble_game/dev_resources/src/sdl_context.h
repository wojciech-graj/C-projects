#ifndef SDL_CONTEXT_H
#define SDL_CONTEXT_H

#include <SDL2/SDL.h>

typedef struct SDLContext {
	const Uint8 *keystates;
	SDL_Window *window;
	SDL_GLContext main_context;
} SDLContext;

#endif
