#ifndef CONTEXT_H
#define CONTEXT_H

#include <stdbool.h>
#include <stdlib.h>

#include "global.h"
#include "object.h"
#include "config.h"

typedef struct Context {
	int resolution_index;
	bool resize;

	int gamestate;
	int timer;

	bool quit;

	short height;
	short width;
	float (*level)[5];
	float (*projection)[4];
	bool (*flat)[2];
	bool *on_screen;
	unsigned char (*floor_colors)[3];
	unsigned char left_color[3];
	unsigned char right_color[3];

	float input[2];
	bool can_control;

	float scroll_offset[2];
	bool scroll;

	short num_textures;
	unsigned int *textures;

	short num_objects;
	Object *objects;

	Config *config;
} Context;

Context *init_context(Config *config);
void delete_context(Context *context);

#endif
