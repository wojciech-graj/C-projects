#include "level.h"

static void read_marble(Context *context, FILE *file, int object_index)
{
	unsigned char color[3];
	assert(fread(color, sizeof(unsigned char), 3, file) == 3);
	context->objects[object_index].marble = init_marble(context, color);
}

static void read_sprite(Context *context, FILE *file, int object_index)
{
	int texture_index;
	assert(fread(&texture_index, sizeof(int), 1, file) == 1);
	int frame_time;
	assert(fread(&frame_time, sizeof(int), 1, file) == 1);
	bool transforms[3];
	assert(fread(transforms, sizeof(bool), 3, file) == 3);
	float corner_projections[4][2];
	assert(fread(corner_projections, sizeof(float), 8, file) == 8);
	//TODO: ADD PHYSICS PROCESS TO LEVEL FILE
	context->objects[object_index].sprite = init_sprite((frame_time == 0) ? NULL : &physics_process_animated_sprite,
		corner_projections, texture_index, frame_time, transforms[0], transforms[1], transforms[2]);
}

static void read_collision_area(Context *context, FILE *file, int object_index)
{
	bool can_move_over;
	assert(fread(&can_move_over, sizeof(bool), 1, file) == 1);
	float corner_positions[4][2];
	assert(fread(corner_positions, sizeof(float), 8, file) == 8);
	context->objects[object_index].collision_area = init_collision_area(NULL, corner_positions, can_move_over);
}

static void read_area(Context *context, FILE *file, int object_index)
{
	short tile_positions[4][2];
	assert(fread(tile_positions, sizeof(short), 8, file) == 8);
	int sprite_index;
	assert(fread(&sprite_index, sizeof(int), 1, file) == 1);
	Sprite *sprite = (sprite_index != -1) ? context->objects[sprite_index].sprite : NULL;
	int collision_area_index;
	assert(fread(&collision_area_index, sizeof(int), 1, file) == 1);
	CollisionArea *collision_area = (collision_area_index != -1) ? context->objects[sprite_index].collision_area : NULL;
	context->objects[object_index].area = init_area(context, NULL, sprite, collision_area, tile_positions);
}

static void read_point(Context *context, FILE *file, int object_index)
{
	int sprite_index;
	assert(fread(&sprite_index, sizeof(int), 1, file) == 1);
	Sprite *sprite = (sprite_index != -1) ? context->objects[sprite_index].sprite : NULL;
	short tile_position[2];
	assert(fread(tile_position, sizeof(short), 2, file) == 2);
	int tile_index = tile_position[X] + tile_position[Y] * context->width;
	float y;
	assert(fread(&y, sizeof(float), 1, file) == 1);
	float z = y / context->height;
	context->objects[object_index].point = init_point(NULL, sprite, tile_index, z);
}

void load_level(char *filename, Context *context)
{
	FILE *file = fopen(filename, "rb");
	assert(file);
	short buffer;

	assert(fread(&(context->width), sizeof(short), 1, file) == 1);
	assert(fread(&(context->height), sizeof(short), 1, file) == 1);
	int level_size = context->height * context->width;
	context->on_screen = malloc(sizeof(bool) * level_size);

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

	assert(fread(&(context->num_objects), sizeof(int), 1, file) == 1);
	context->objects = init_objectlist(context->num_objects);

	for(i = 0; i < context->num_objects; i++) //TODO: CHANGE
	{
		int type;
		assert(fread(&type, sizeof(int), 1, file) == 1);
		switch(type)
		{
			case MARBLE:
			read_marble(context, file, i);
			break;
			case SPRITE:
			read_sprite(context, file, i);
			break;
			case COLLISIONAREA:
			read_collision_area(context, file, i);
			break;
			case AREA:
			read_area(context, file, i);
			break;
			case POINT:
			read_point(context, file, i);
			break;
		}
	}

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

	if(tile_position) {
		float offset = .5 * ((abs(posx) + abs(posy)) % 2 == 0);
		double i;
		tile_position[X] = modf(position[X] + offset, &i);
		tile_position[Y] = modf(position[Y] + offset, &i);
	}
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
