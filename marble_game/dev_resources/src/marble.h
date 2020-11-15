#ifndef MARBLE_H
#define MARBLE_H

#include <stdlib.h>
#include <stdbool.h>

#include "global.h"
#include "context.h"
#include "object.h"
#include "math_functions.h"
#include "level.h"

#define GRAVITY_ACCELERATION .01
#define FRICTION .02
#define MARBLE_ACCELERATION .003
#define MAX_DELTA_Z .3

typedef struct Marble {
	void (*physics_process)(Context*, Object*);
	float position[3];
	int tile_index;
	float tile_position[2];
	float velocity[3];
	float radius;
	bool in_air;
} Marble;

Marble *init_marble(Context *context);
void physics_process_marble(Context *context, Object *object);

#endif
