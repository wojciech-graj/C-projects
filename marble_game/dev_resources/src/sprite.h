#ifndef SPRITE_H
#define SPRITE_H

#include <stdbool.h>
#include <string.h>

#include "context.h"
#include "global.h"

typedef struct Sprite {
	void (*physics_process)(Context*, Object*);
	void (*delete)(Object*);
	int type;
	int texture_index;
	int frame;
	int frame_time;
	bool rotate;
	bool flip_x;
	bool flip_y;
	int tile_index;
	float corner_projections[4][2];
} Sprite;

Sprite *init_sprite(void (*physics_process)(Context*, Object*), float corner_projections[4][2], int texture_index, int frame_time, bool rotate, bool flip_x, bool flip_y);
void physics_process_animated_sprite(Context *context, Object *object);

#endif
