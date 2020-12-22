#ifndef AREA_H
#define AREA_H

#include "global.h"
#include "object.h"

typedef struct Level Level;
typedef struct Sprite Sprite;
typedef struct CollisionArea CollisionArea;

typedef struct Area Area;

typedef struct Area {
	DEFAULT_OBJECT_PARAMS
	Sprite *sprite;
	CollisionArea *collision_area;
	int corner_tile_indexes[4];
	float corner_positions[4][2];
	int side_lengths[2];
	float tile_side_lengths[2]; //fraction of side which a tile takes up
} Area;

Area *init_area(Level *level, void (*physics_process)(Context*, Object), Sprite *sprite, CollisionArea *collision_area, short tile_positions[4][2]);

#endif
