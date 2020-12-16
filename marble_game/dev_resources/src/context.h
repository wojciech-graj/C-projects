#ifndef CONTEXT_H
#define CONTEXT_H

#include <stdbool.h>
#include <stdlib.h>

#include "global.h"
#include "object.h"

typedef struct Context {
	short height;
	short width;
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

Context *init_context(void);
void delete_context(Context *context);

#endif
