#include "level.h"

void load_level(char *filename, Context *context)
{
	FILE *file = fopen(filename, "rb");
	assert(file);
	short buffer;

	assert(fread(&(context->width), sizeof(short), 1, file) == 1);
	assert(fread(&(context->height), sizeof(short), 1, file) == 1);
	int level_size = context->height * context->width;

	context->level = malloc(sizeof(float) * level_size * 5);
	int i;
	for(i = 0; i < level_size; i++)
	{
		int j;
		for(j = 0; j < 5; j++)
		{
			assert(fread(&buffer, sizeof(short), 1, file) == 1);
			context->level[i][j] = buffer/2.f;
		}
	}

	calculate_level_projection(context);

	context->floor_colors = malloc(sizeof(unsigned char) * level_size * 3);
	assert(fread(context->floor_colors, sizeof(unsigned char), level_size * 3, file) == (size_t) (level_size * 3));

	assert(fread(context->left_color, sizeof(unsigned char), 3, file) == 3);
	assert(fread(context->right_color, sizeof(unsigned char), 3, file) == 3);

	context->objects = init_objectlist(NUM_OBJECTS);

	unsigned char marble_color[3] = {0, 255, 0};
	context->objects[ID_PLAYER_MARBLE].marble = init_marble(context, marble_color);

	short tile_positions[4][2];
	assert(fread(tile_positions, sizeof(short), 8, file) == 8);
	//TODO: ADD AREA PARAMS TO SERIALIZER
	context->objects[ID_GOAL].area = init_area(context, T_GOAL, tile_positions, true, false, false);

	fclose(file);
}

//calculates tile_index and tile_position
//x_pos and y_pos are coordinates from position, but with axes rotated 45 degrees anticlockwise, y shifted by .5, scaled up by sqrt(2)
//tile_index and tile_position can be easily calculated using the rotated coordinates
/* graph of tile indexes after rotation of axes:
x
2      2 5
1    1 4 8
0  0 3 7
-1   6
   0 1 2 3 y
*/
void calculate_tile(float *position, int *tile_index, float *tile_position, Context *context)
{
	int posx = floor(position[X] - position[Y] + .5f);
	int posy = floor(position[X] + position[Y] + .5f);

	*tile_index = context->width * (posy - posx) + posx + floor((posy - posx)/2.);

	float offset = .5 * ((abs(posx) + abs(posy)) % 2 == 0);
	double i;
	tile_position[X] = modf(position[X] + offset, &i);
	tile_position[Y] = modf(position[Y] + offset, &i);
}

void calculate_level_projection(Context *context)
{
	context->projection = malloc(sizeof(float) * context->height * context->width * 4);
	int tile_position[2];
	for(tile_position[Y] = 0; tile_position[Y] < context->height; tile_position[Y]++)
	{
		for(tile_position[X] = 0; tile_position[X] < context->width; tile_position[X]++)
		{
			int i = tile_position[Y] * context->width + tile_position[X];
			context->projection[i][L] = context->level[i][L]/2.f - tile_position[Y]/4.f;
			context->projection[i][T] = context->level[i][T]/2.f - tile_position[Y]/4.f + .25f;
			context->projection[i][R] = context->level[i][R]/2.f - tile_position[Y]/4.f;
			context->projection[i][B] = context->level[i][B]/2.f - tile_position[Y]/4.f - .25f;
		}
	}
}
