#ifndef MARBLE_GAME_H
#define MARBLE_GAME_H

#include <assert.h>
#include <math.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>

#include <GL/gl.h>
#include <GL/glu.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 640
#define level_width 5
#define level_height 10
#define MIN_HEIGHT -2
#define M_TAO (M_PI * 2)

const float RED[] = {1, 0, 0};
const float GREEN[] = {0, 1, 0};
const float BLUE[] = {0, 0, 1};

SDL_Window *window;
SDL_GLContext main_context;

enum directions{l, t, r, b};

float level_projection[level_width * level_height][4];
float level[][4] = {
	{3, 3, 3, 3}, {3, 3, 3, 3}, {3, 3, 3, 3}, {3, 3, 3, 3}, {3, 3, 3, 3},
			{3, 3, 3, 3}, {5, 5, 5, 5}, {3, 3, 3, 3}, {3, 3, 3, 3}, {3, 3, 4, 3},
	{3, 3, 3, 3}, {3, 5, 5, 3}, {3, 3, 3, 3}, {6, 6, 6, 6}, {4, 3, 3, 3},
			{3, 3, 3, 3}, {3, 3, 3, 3}, {5, 5, 5, 5}, {3, 3, 3, 3}, {3, 3, 3, 3},
	{3, 3, 3, 3}, {3, 3, 3, 3}, {3, 3, 3, 3}, {3, 3, 3, 3}, {3, 3, 3, 3},
		{3, 3, 3, 3}, {3, 3, 3, 3}, {3, 3, 3, 3}, {3, 3, 3, 3}, {3, 3, 3, 3},
	{3, 3, 3, 3}, {3, 3, 3, 3}, {3, 3, 3, 3}, {3, 3, 3, 3}, {3, 3, 3, 3},
		{3, 3, 3, 3}, {3, 3, 3, 3}, {3, 3, 3, 3}, {3, 3, 3, 3}, {3, 3, 3, 3},
	{3, 3, 3, 3}, {3, 3, 3, 3}, {3, 3, 3, 3}, {3, 3, 3, 3}, {3, 3, 3, 3},
		{3, 3, 3, 3}, {3, 3, 3, 3}, {3, 3, 3, 3}, {3, 3, 3, 3}, {3, 3, 3, 3}};

float ball_pos[2];
float frac_ball_pos[2];
int ball_tile;
float ball_vel[2] = {0, 0};
int num_ball_points = 12;
float ball_rad = .2;

#endif
