#ifndef MARBLE_H
#define MARBLE_H

#include <stdlib.h>
#include <stdbool.h>

#include "global_constants.h"
#include "context.h"
#include "math_functions.h"
#include "level.h"

#define GRAVITY_ACCELERATION .01
#define FRICTION .02
#define MARBLE_ACCELERATION .003
#define MAX_DELTA_Z .3

typedef struct Marble Marble;
typedef struct Marble {
	float position[3];
	int tile_index;
	float tile_position[2];
	float velocity[3];
	float radius;
	bool in_air;
	void (*physics_process)(Marble*, Context*);
} Marble;

Marble *init_marble(Context *level);
void physics_process_marble(Marble *marble, Context *context);

#endif
