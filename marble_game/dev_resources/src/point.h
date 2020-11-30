#ifndef POINT_H
#define POINT_H

#include "context.h"
#include "global.h"
#include "sprite.h"

typedef struct Point {
	void (*physics_process)(Context*, Object);
	void (*delete)(Object);
	int type;
	Sprite *sprite;
	int tile_index;
	float z;
} Point;

Point *init_point(void (*physics_process)(Context*, Object), Sprite *sprite, int tile_index, float z);
void physics_process_point(Context *context, Object object);
void delete_point(Object object);

#endif
