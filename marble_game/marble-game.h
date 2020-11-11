#ifndef MARBLE_GAME_H
#define MARBLE_GAME_H

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

#include <SDL2/SDL.h>

#include <GL/gl.h>
#include <GL/glu.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 640
#define TILES_ON_SCREEN 8
#define SCROLL_BORDER 2
#define EDGE_HEIGHT 8
#define M_TAO (M_PI * 2)
#define FPS 30
#define FRAMETIME (1000 / FPS)
#define NUM_CIRCLE_POINTS 16
#define GRAVITY_ACCELERATION .01
#define FRICTION .02
#define MARBLE_ACCELERATION .003
#define MAX_DELTA_Z .3
#define NUM_TEXTURES 1

#define MIN(a, b) ((a < b) ? a : b)
#define MAX(a, b) ((a > b) ? a : b)
#define ON_SCREEN_Y(posy) (posy >= -scroll_offset[y]/2.\
	&& posy <= TILES_ON_SCREEN -scroll_offset[y]/2.)
#define ON_SCREEN_X(posx) (posx >= 0 + scroll_offset[x]\
	&& posx <= TILES_ON_SCREEN + scroll_offset[x])

enum coordinates{x, y, z};

const float GREEN[] = {0, 1, 0};
unsigned char floor_color[3];
unsigned char left_color[3];
unsigned char right_color[3];

const Uint8 *keystates = NULL;
SDL_Window *window = NULL;
SDL_GLContext main_context = NULL;

enum tile_directions{l, t, r, b, d};
short level_height;
short level_width;
float (*level)[5];

enum texture_names{T_GOAL};
unsigned int textures[NUM_TEXTURES];

typedef struct Marble Marble;
Marble *player_marble = NULL;

float scroll_offset[2] = {0, 0};

#endif
