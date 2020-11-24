#ifndef AREA_H
#define AREA_H

#include <stdbool.h>
#include <math.h>

#include "global.h"
#include "context.h"
#include "object.h"
#include "math_functions.h"

typedef struct Area {
	void (*physics_process)(Context*, Object*);
	int texture_index;
	bool rotate;
	bool flip_x;
	bool flip_y;
	float corner_positions[4][2];
	float corner_projections[4];
	float vectors[2][2]; //LT, TR
	float dot_products[2]; //dot(LT, LT), dot(TR, TR)
	int side_lengths_sqr[2]; //squared side lengths
	float half_tile_side_lengths[2]; //fraction of side which half a tile would take up
} Area;

Area *init_area(Context *context, int texture_index, short tile_positions[4][2], bool rotate, bool flip_x, bool flip_y);
bool in_area(Area *area, float *position, float *area_position);

#endif
