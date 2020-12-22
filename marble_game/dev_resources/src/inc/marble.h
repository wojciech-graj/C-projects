#ifndef MARBLE_H
#define MARBLE_H

#include <stdbool.h>

#include "global.h"
#include "object.h"

typedef struct Context Context;

typedef struct Marble Marble;

typedef struct Marble {
	DEFAULT_OBJECT_PARAMS
	float position[3];
	int tile_index;
	int num_collision_points;
	float tile_frac_position[2];
	float velocity[3];
	float radius;
	bool in_air;
	unsigned char color[3];
} Marble;

Marble *init_marble(Context *context, unsigned char color[3], float radius, int num_collision_points);
void physics_process_marble(Context *context, Object object);

#endif
