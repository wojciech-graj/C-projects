#ifndef AREA_H
#define AREA_H

#include <math.h>
#include <stdbool.h>

#include "context.h"
#include "global.h"
#include "math_functions.h"
#include "object.h"
#include "sprite.h"

typedef struct Area {
	void (*physics_process)(Context*, Object);
	void (*delete)(Object);
	int type;
	Sprite *sprite;
	int corner_tile_indexes[4];
	float corner_positions[4][2];
	float vectors[2][2]; //LT, TR
	float dot_products[2]; //dot(LT, LT), dot(TR, TR)
	int side_lengths[2];
	float tile_side_lengths[2]; //fraction of side which a tile would take up
} Area;

Area *init_area(Context *context, void (*physics_process)(Context*, Object), Sprite *sprite, short tile_positions[4][2]);
bool in_area(Area *area, float *position);
void delete_area(Object object);

#endif
