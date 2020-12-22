#ifndef LEVEL_P_H
#define LEVEL_P_H

#include <assert.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "inc/area.h"
#include "inc/collision_area.h"
#include "inc/context.h"
#include "inc/global.h"
#include "inc/marble.h"
#include "inc/math_functions.h"
#include "inc/object.h"
#include "inc/point.h"
#include "inc/sprite.h"

void read_marble(Context *context, FILE *file, int object_index);
void read_sprite(Context *context, FILE *file, int object_index);
void read_collision_area(Context *context, FILE *file, int object_index);
void read_area(Context *context, FILE *file, int object_index);
void read_point(Context *context, FILE *file, int object_index);
void read_colors(Context *context, FILE *file, int level_size);
void read_objects(Context *context, FILE *file);

#endif
