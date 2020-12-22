#ifndef POINT_H
#define POINT_H

#include "global.h"
#include "object.h"

typedef struct Context Context;
typedef struct Sprite Sprite;
typedef struct CollisionArea CollisionArea;

typedef struct Point Point;

typedef struct Point {
	DEFAULT_OBJECT_PARAMS
	Sprite *sprite;
	CollisionArea *collision_area;
	int tile_index;
	float z;
} Point;

Point *init_point(void (*physics_process)(Context*, Object), Sprite *sprite, int tile_index, float z);
void physics_process_point(Context *context, Object object);

#endif
