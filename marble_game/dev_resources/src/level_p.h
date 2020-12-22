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

void read_marble(Level *level, FILE *file, int object_index);
void read_sprite(Level *level, FILE *file, int object_index);
void read_collision_area(Level *level, FILE *file, int object_index);
void read_area(Level *level, FILE *file, int object_index);
void read_point(Level *level, FILE *file, int object_index);
void read_colors(Level *level, FILE *file, int level_size);
void read_objects(Level *level, FILE *file);

#endif
