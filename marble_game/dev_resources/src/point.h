#ifndef POINT_H
#define POINT_H

#include "collision_area.h"
#include "context.h"
#include "global.h"
#include "sprite.h"

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
