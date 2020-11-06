#ifndef MARBLE_GAME_H
#define MARBLE_GAME_H

#include <assert.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>

#include <GL/gl.h>
#include <GL/glu.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 640
#define MIN_HEIGHT -2
#define M_TAO (M_PI * 2)
#define FPS 30
#define FRAMETIME (1000 / FPS)
#define NUM_CIRCLE_POINTS 16
#define GRAVITY_ACCELERATION .01
#define FRICTION .02
#define MARBLE_ACCELERATION .003
#define MAX_DELTA_Z .2

short level_height;
short level_width;

unsigned char floor_color[3];
unsigned char left_color[3];
unsigned char right_color[3];

const float GREEN[] = {0, 1, 0};

SDL_Window *window = NULL;
SDL_GLContext main_context = NULL;
const Uint8 *keystates = NULL;

enum tile_directions{l, t, r, b};
enum coordinates{x, y, z};

float (*level)[4];

typedef struct Marble Marble;

Marble *player_marble = NULL;

#endif
