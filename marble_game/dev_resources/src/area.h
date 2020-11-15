#ifndef AREA_H
#define AREA_H

#include <stdbool.h>

#include "global.h"
#include "context.h"
#include "object.h"
#include "math_functions.h"

typedef struct Area {
	void (*physics_process)(Context*, Object*);
	int texture_index;
	float positions[4][2];
	int tile_indexes[4];
} Area;

Area *init_area(int texture_index, short tile_positions[4][2], short level_width);
bool area_colliding(Area *area, float *position);

#endif
