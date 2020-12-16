#ifndef MARBLE_H
#define MARBLE_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "context.h"
#include "global.h"
#include "level.h"
#include "math_functions.h"
#include "object.h"

#define GRAVITY_ACCELERATION .01f
#define FRICTION .02f
#define MARBLE_ACCELERATION .003f
#define MAX_DELTA_Z .1f
#define COLLISION_ANGLE_PADDING (M_TAU / 8.f)

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
