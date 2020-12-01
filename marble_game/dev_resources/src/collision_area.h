#ifndef COLLISION_AREA_H
#define COLLISION_AREA_H

//TODO:ALPHABETICAL INCLUDE TOOL
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "global.h"
#include "math_functions.h"
#include "object.h"

typedef struct Context Context;
typedef struct CollisionArea {
	DEFAULT_OBJECT_PARAMS
	float corner_positions[4][2];
	float vectors[2][2]; //LT, TR
	float dot_products[2]; //dot(LT, LT), dot(TR, TR)
} CollisionArea;

CollisionArea *init_collision_area(void (*physics_process)(Context*, Object), float corner_positions[4][2]);
bool in_collision_area(CollisionArea *collision_area, float *position);
void physics_process_collision(Context *context, Object object);

#endif
