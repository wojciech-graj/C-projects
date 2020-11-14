#ifndef AREA_H
#define AREA_H

#include <stdbool.h>

#include "global_constants.h"
#include "context.h"
#include "math_functions.h"
#include "level.h"

typedef struct Area Area;
typedef struct Area {
	unsigned int texture;
	float positions[4][2];
	int tile_indexes[4];
} Area;

Area *init_area(unsigned int texture, short tile_positions[4][2], short level_width);
bool area_colliding(Area *area, float *position);

#endif
