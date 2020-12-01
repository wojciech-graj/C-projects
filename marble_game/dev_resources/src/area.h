#ifndef AREA_H
#define AREA_H

#include <stdbool.h>

#include "context.h"
#include "global.h"
#include "math_functions.h"
#include "object.h"

typedef struct Sprite Sprite;
typedef struct CollisionArea CollisionArea;
typedef struct Area {
	DEFAULT_OBJECT_PARAMS
	Sprite *sprite;
	CollisionArea *collision_area;
	int corner_tile_indexes[4];
	float corner_positions[4][2];
	int side_lengths[2];
	float tile_side_lengths[2]; //fraction of side which a tile takes up
} Area;

Area *init_area(Context *context, void (*physics_process)(Context*, Object), Sprite *sprite, CollisionArea *collision_area, short tile_positions[4][2]);

#endif
