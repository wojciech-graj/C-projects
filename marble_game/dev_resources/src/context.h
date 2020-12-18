#ifndef CONTEXT_H
#define CONTEXT_H

#include <stdbool.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#include "global.h"
#include "object.h"

//TODO: ORGANIZE
typedef struct Context {
	int resolution_index;
	short height;
	short width;
	int gamestate;
	bool resize;
	bool quit;
	float (*level)[5];
	float (*projection)[4];
	bool (*flat)[2];
	bool *on_screen;
	int timer;
	float input[2];
	bool can_control;
	float scroll_offset[2];
	bool scroll;
	unsigned char (*floor_colors)[3];
	unsigned char left_color[3];
	unsigned char right_color[3];
	unsigned int textures[NUM_TEXTURES];
	short num_objects;
	Object *objects;
} Context;

typedef struct SDLContext { //TODO: SEPARATE FILE
	const Uint8 *keystates;
	SDL_Window *window;
	SDL_GLContext main_context;
} SDLContext;

Context *init_context(void);
void delete_context(Context *context);

#endif
