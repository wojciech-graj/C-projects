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
#include "menu.h"
#include "sprite.h"

#define Y_PADDING 2
#define EDGE_HEIGHT 8
#define NUM_CIRCLE_POINTS 16

#define ON_SCREEN_Y(posy, scroll_offset) (posy >= -scroll_offset[Y]/2.f - Y_PADDING\
	&& posy <= TILES_ON_SCREEN_Y - scroll_offset[Y]/2.f + Y_PADDING)
#define ON_SCREEN_X(posx, scroll_offset) (posx >= scroll_offset[X]\
	&& posx <= TILES_ON_SCREEN_X + scroll_offset[X])
#define START_FOR_EACH_TILE_ON_SCREEN int tile_position[2];\
	for(tile_position[Y] = 0; tile_position[Y] < context->height; tile_position[Y]++) {\
		int offset = tile_position[Y] % 2;\
		float z = tile_position[Y] / (float) context->height;\
		for(tile_position[X] = 0; tile_position[X] < context->width; tile_position[X]++) {\
			int tile_index = tile_position[Y] * context->width + tile_position[X];\
			if(context->on_screen[tile_index]) {\
				float *tile = context->projection[tile_index];\
				float x_l = tile_position[X] - .5f + offset/2.f;\
				float x_m = tile_position[X] + offset/2.f;\
				float x_r = tile_position[X] + .5f + offset/2.f;
#define END_FOR_EACH_TILE_ON_SCREEN }}}

void draw_game(SDLContext *sdl_context, Context *context);
void draw_menu(SDLContext *sdl_context, Context *context, MenuContext *menu_context);

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
	glColor3ub(mini(maxi(0, floor_color[0] * cmul), 255), //floor_color does not have to be converted to int to prevent overflow because of integer promotion
		mini(maxi(0, floor_color[1] * cmul), 255),
		mini(maxi(0, floor_color[2] * cmul), 255));
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
