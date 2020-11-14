#include "level.h"

void load_level(char *filename, Context *context)
{
	FILE *file = fopen(filename, "rb");
	assert(file);
	short buffer;

	fread(&(context->width), sizeof(short), 1, file);
	fread(&(context->height), sizeof(short), 1, file);

	context->level = malloc(sizeof(float) * context->height * context->width * 5);

	int i;
	for(i = 0; i < context->width * context->height; i++)
	{
		int j;
		for(j = 0; j < 5; j++)
		{
			fread(&buffer, sizeof(short), 1, file);
			context->level[i][j] = buffer/2.;
		}
	}

	calculate_level_projection(context);

	fread(context->floor_color, sizeof(unsigned char), 3, file);
	fread(context->left_color, sizeof(unsigned char), 3, file);
	fread(context->right_color, sizeof(unsigned char), 3, file);

	short tile_positions[4][2];
	fread(tile_positions, sizeof(short), 8, file);
	context->goal = init_area(T_GOAL, tile_positions, context->width);

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
	int posx = floor(position[X] - position[Y] + .5);
	int posy = floor(position[X] + position[Y] + .5);

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
			context->projection[i][L] = context->level[i][L]/2. - tile_position[Y]/4.;
			context->projection[i][T] = context->level[i][T]/2. - tile_position[Y]/4. + .25;
			context->projection[i][R] = context->level[i][R]/2. - tile_position[Y]/4.;
			context->projection[i][B] = context->level[i][B]/2. - tile_position[Y]/4. - .25;
		}
	}
}
