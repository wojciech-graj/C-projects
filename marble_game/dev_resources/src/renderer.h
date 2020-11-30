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
#define ON_SCREEN_X(posx, scroll_offset) (posx >= scroll_offset[X]\
	&& posx <= TILES_ON_SCREEN + scroll_offset[X])

typedef struct SDL_Context {
	const Uint8 *keystates;
	SDL_Window *window;
	SDL_GLContext main_context;
} SDL_Context;

void draw(SDL_Context *sdl_context, Context *context);

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

static inline void TILE_TRIANGLE(float x_m, float x_s, float tile_b, float tile_s, float tile_t, float z, float cmul, unsigned char *floor_color)
{
	glColor3ub(imin(imax(0, floor_color[0] * cmul), 255), //floor_color does not have to be converted to int to prevent overflow because of integer promotion
		imin(imax(0, floor_color[1] * cmul), 255),
		imin(imax(0, floor_color[2] * cmul), 255));
	glVertex3f(x_m, tile_b, z);
	glVertex3f(x_s, tile_s, z);
	glVertex3f(x_m, tile_t, z);
}

static inline void TILE_SIDE(float x_m, float x_s, float z, float tile_b, float tile_s, float bottom_tile_t, float bottom_tile_s, unsigned char *color)
{
	glColor3ubv(color);
	glVertex3f(x_m, tile_b, z);
	glVertex3f(x_s, tile_s, z);
	glVertex3f(x_s, bottom_tile_t, z);
	glVertex3f(x_m, bottom_tile_s, z);
}

static inline void TILE_OUTLINE(float x_l, float x_m, float x_r, float z, float *tile)
{
	glVertex3f(x_l, tile[L], z);
	glVertex3f(x_m, tile[T], z);
	glVertex3f(x_r, tile[R], z);
	glVertex3f(x_m, tile[B], z);
}

#endif
