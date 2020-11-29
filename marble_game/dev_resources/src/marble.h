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
#define MAX_DELTA_Z .3f

typedef struct Marble {
	void (*physics_process)(Context*, Object);
	void (*delete)(Object);
	int type;
	float position[3];
	int tile_index;
	float tile_position[2];
	float velocity[3];
	float radius;
	bool in_air;
	unsigned char color[3];
} Marble;

Marble *init_marble(Context *context, unsigned char color[3]);
void physics_process_marble(Context *context, Object object);
void delete_marble(Object object);

#endif
