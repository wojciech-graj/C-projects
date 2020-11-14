#ifndef CONTEXT_H
#define CONTEXT_H

#include <stdlib.h>
#include <stdbool.h>

#include "global_constants.h"

typedef struct Marble Marble;
typedef struct Area Area;
typedef struct Context {
	short height;
	short width;
	float (*level)[5];
	float (*projection)[4];
	float scroll_offset[2];
	bool scroll;
	unsigned char floor_color[3];
	unsigned char left_color[3];
	unsigned char right_color[3];
	unsigned int textures[NUM_TEXTURES];
	Marble *player_marble;
	Area *goal;
} Context;

Context *init_context(void);
void delete_context(Context *context);

#endif
