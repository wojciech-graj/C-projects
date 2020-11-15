#ifndef RENDERER_H
#define RENDERER_H

#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "global.h"
#include "math_functions.h"
#include "context.h"
#include "marble.h"
#include "level.h"
#include "area.h"

#define EDGE_HEIGHT 8
#define M_TAO 6.28318531f
#define NUM_CIRCLE_POINTS 12

#define ON_SCREEN_Y(posy, scroll_offset) (posy >= -scroll_offset[Y]/2.f\
	&& posy <= TILES_ON_SCREEN -scroll_offset[Y]/2.f)
#define ON_SCREEN_X(posx, scroll_offset) (posx >= 0 + scroll_offset[X]\
	&& posx <= TILES_ON_SCREEN + scroll_offset[X])

typedef struct SDL_Context {
	const Uint8 *keystates;
	SDL_Window *window;
	SDL_GLContext main_context;
} SDL_Context;

void draw(SDL_Context *sdl_context, Context *context);

#endif
