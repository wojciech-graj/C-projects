#ifndef RENDERER_H
#define RENDERER_H

#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>

#include "area.h"
#include "context.h"
#include "global.h"
#include "level.h"
#include "marble.h"
#include "math_functions.h"
#include "sprite.h"

#define Y_PADDING 2
#define EDGE_HEIGHT 8
#define M_TAO 6.28318531f
#define NUM_CIRCLE_POINTS 12

#define ON_SCREEN_Y(posy, scroll_offset) (posy >= -scroll_offset[Y]/2.f - Y_PADDING\
	&& posy <= TILES_ON_SCREEN - scroll_offset[Y]/2.f + Y_PADDING)
#define ON_SCREEN_X(posx, scroll_offset) (posx >= 0 + scroll_offset[X]\
	&& posx <= TILES_ON_SCREEN + scroll_offset[X])

static inline void START_TEXTURE(int texture)
{
	glBindTexture(GL_TEXTURE_2D, texture);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_QUADS);
}

static inline void END_TEXTURE(void)
{
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

typedef struct SDL_Context {
	const Uint8 *keystates;
	SDL_Window *window;
	SDL_GLContext main_context;
} SDL_Context;

void draw(SDL_Context *sdl_context, Context *context);

#endif
