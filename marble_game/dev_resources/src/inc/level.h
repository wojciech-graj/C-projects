#ifndef LEVEL_H
#define LEVEL_H

#include <stdbool.h>

typedef union Object Object;

typedef struct Level Level;

typedef struct Level {
	float scroll_offset[2];
	bool scroll;

	short height;
	short width;
	float (*tiles)[5];
	float (*projection)[4];
	bool (*flat)[2];
	bool *on_screen;
	unsigned char (*floor_colors)[3];
	unsigned char left_color[3];
	unsigned char right_color[3];

	short num_objects;
	Object *objects;
} Level;

Level *load_level(char *filename);
void delete_level(Level *level);
void calculate_tile(float *position, int *tile_index, float *tile_position, Level *level);
void calculate_level_projection(Level *level);
bool colliding_with_level(Level *level, float *position, float max_z, float height, int unconsidered_tile_index);

#endif
