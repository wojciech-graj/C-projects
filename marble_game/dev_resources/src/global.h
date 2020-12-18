#ifndef GLOBAL_H
#define GLOBAL_H

#ifdef DEBUG
#include <stdio.h>
#define DBG_LOG(format, val) printf(format, val)
#else
#define DBG_LOG(format, val)
#endif

#define TILES_ON_SCREEN_X 8
#define TILES_ON_SCREEN_Y 6
#define SCROLL_BORDER 2
#define DEFAULT_RESOLUTION_INDEX 5
#define FPS 30
#define FRAMETIME (1000 / FPS)
#define NUM_RESOLUTIONS 15
#define M_TAU 6.28318531f

#define DEFAULT_OBJECT_PARAMS void (*physics_process)(Context*, Object);\
	int type;

enum texture_names{T_GOAL, T_FLAG_RED, T_TEXT};
enum object_types{MARBLE, AREA, POINT, SPRITE, COLLISIONAREA};
enum coordinates{X, Y, Z};
enum tile_directions{L, T, R, B, D};
enum gamestates{GAME, MENU};

extern float ORIGIN[2];
extern int NUM_TEXTURE_FRAMES[];
extern int RESOLUTIONS[][2];

#endif
