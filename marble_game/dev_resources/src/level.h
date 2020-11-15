#ifndef LEVEL_H
#define LEVEL_H

#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>

#include "global.h"
#include "context.h"
#include "object.h"
#include "marble.h"
#include "area.h"

void load_level(char *filename, Context *context);
void calculate_tile(float *position, int *tile_index, float *tile_position, Context *context);
void calculate_level_projection(Context *context);

#endif
