#ifndef GLOBAL_H
#define GLOBAL_H

#define TILES_ON_SCREEN 8
#define SCROLL_BORDER 2
#define NUM_TEXTURES 3
#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 640
#define FPS 30
#define FRAMETIME (1000 / FPS)

#ifdef DEBUG
#include <stdio.h>
#define DBG_LOG(format, val) printf(format, val)
#else
#define DBG_LOG(format, val)
#endif

#define M_TAU 6.28318531f

#define DEFAULT_OBJECT_PARAMS void (*physics_process)(Context*, Object);\
	int type;

enum texture_names{T_GOAL, T_FLAG_RED, T_TEXT};
enum object_types{MARBLE, AREA, POINT, SPRITE, COLLISIONAREA};
enum coordinates{X, Y, Z};
enum tile_directions{L, T, R, B, D};
enum gamestates{GAME, MENU};
enum menu_presets{MENU_MAIN};
enum menu_context_presets{MENU_CONTEXT_MAIN};

extern float ORIGIN[2];
extern int NUM_TEXTURE_FRAMES[NUM_TEXTURES];

#endif
