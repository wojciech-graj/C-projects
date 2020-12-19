#ifndef LEVEL_H
#define LEVEL_H

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include "area.h"
#include "collision_area.h"
#include "context.h"
#include "global.h"
#include "marble.h"
#include "math_functions.h"
#include "object.h"
#include "point.h"
#include "sprite.h"

void load_level(char *filename, Context *context);
void calculate_tile(float *position, int *tile_index, float *tile_position, Context *context);
void calculate_level_projection(Context *context);
bool colliding_with_level(Context *context, float *position, float max_z, float height, int unconsidered_tile_index);

#endif
