#ifndef COLLISION_AREA_H
#define COLLISION_AREA_H

#include <stdbool.h>

#include "global.h"
#include "object.h"

typedef struct Context Context;

typedef struct CollisionArea CollisionArea;

typedef struct CollisionArea {
	DEFAULT_OBJECT_PARAMS
	bool colliding;
	bool can_move_over;
	float corner_positions[4][2];
	float vectors[2][2]; //LT, TR
	float dot_products[2]; //dot(LT, LT), dot(TR, TR)
} CollisionArea;

CollisionArea *init_collision_area(void (*physics_process)(Context*, Object), float corner_positions[4][2], bool can_move_over);
bool in_collision_area(CollisionArea *collision_area, float *position);

#endif
