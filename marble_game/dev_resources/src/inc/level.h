#ifndef LEVEL_H
#define LEVEL_H

#include <stdbool.h>

typedef struct Context Context;

void load_level(char *filename, Context *context);
void calculate_tile(float *position, int *tile_index, float *tile_position, Context *context);
void calculate_level_projection(Context *context);
bool colliding_with_level(Context *context, float *position, float max_z, float height, int unconsidered_tile_index);

#endif
