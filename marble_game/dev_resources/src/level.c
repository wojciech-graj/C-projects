#include "inc/level.h"
#include "level_p.h"

void read_marble(Level *level, FILE *file, int object_index)
{
	unsigned char color[3];
	assert(fread(color, sizeof(unsigned char), 3, file) == 3);
	level->objects[object_index].marble = init_marble(level, color, 0.2f, 4);
}

void read_sprite(Level *level, FILE *file, int object_index)
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
	level->objects[object_index].sprite = init_sprite((frame_time == 0) ? NULL : &physics_process_animated_sprite,
		corner_projections, texture_index, frame_time, transforms[0], transforms[1], transforms[2]);
}

void read_collision_area(Level *level, FILE *file, int object_index)
{
	bool can_move_over;
	assert(fread(&can_move_over, sizeof(bool), 1, file) == 1);
	float corner_positions[4][2];
	assert(fread(corner_positions, sizeof(float), 8, file) == 8);
	level->objects[object_index].collision_area = init_collision_area(NULL, corner_positions, can_move_over);
}

void read_area(Level *level, FILE *file, int object_index)
{
	short tile_positions[4][2];
	assert(fread(tile_positions, sizeof(short), 8, file) == 8);
	int sprite_index;
	assert(fread(&sprite_index, sizeof(int), 1, file) == 1);
	Sprite *sprite = (sprite_index != -1) ? level->objects[sprite_index].sprite : NULL;
	int collision_area_index;
	assert(fread(&collision_area_index, sizeof(int), 1, file) == 1);
	CollisionArea *collision_area = (collision_area_index != -1) ? level->objects[sprite_index].collision_area : NULL;
	level->objects[object_index].area = init_area(level, NULL, sprite, collision_area, tile_positions);
}

void read_point(Level *level, FILE *file, int object_index)
{
	int sprite_index;
	assert(fread(&sprite_index, sizeof(int), 1, file) == 1);
	Sprite *sprite = (sprite_index != -1) ? level->objects[sprite_index].sprite : NULL;
	short tile_position[2];
	assert(fread(tile_position, sizeof(short), 2, file) == 2);
	int tile_index = tile_position[X] + tile_position[Y] * level->width;
	float y;
	assert(fread(&y, sizeof(float), 1, file) == 1);
	float z = y / level->height;
	level->objects[object_index].point = init_point(NULL, sprite, tile_index, z);
}

void read_colors(Level *level, FILE *file, int level_size)
{
	level->floor_colors = malloc(sizeof(unsigned char) * level_size * 3);
	assert(fread(level->floor_colors, sizeof(unsigned char), level_size * 3, file) == (size_t) (level_size * 3));
	assert(fread(level->left_color, sizeof(unsigned char), 3, file) == 3);
	assert(fread(level->right_color, sizeof(unsigned char), 3, file) == 3);
}

void read_objects(Level *level, FILE *file)
{
	assert(fread(&(level->num_objects), sizeof(int), 1, file) == 1);
	level->objects = init_objectlist(level->num_objects);

	int i;
	for(i = 0; i < level->num_objects; i++)
	{
		int type;
		assert(fread(&type, sizeof(int), 1, file) == 1);
		switch(type) //TODO: optimize by making an array of functions
		{
			case MARBLE:
			read_marble(level, file, i);
			break;
			case SPRITE:
			read_sprite(level, file, i);
			break;
			case COLLISIONAREA:
			read_collision_area(level, file, i);
			break;
			case AREA:
			read_area(level, file, i);
			break;
			case POINT:
			read_point(level, file, i);
			break;
		}
	}
}

Level *load_level(char *filename)
{
	FILE *file = fopen(filename, "rb");
	assert(file);

	Level *level = malloc(sizeof(Level));
	level->scroll = true;
	level->scroll_offset[X] = 0;
	level->scroll_offset[Y] = 0;

	short buffer;

	assert(fread(&(level->width), sizeof(short), 1, file) == 1);
	assert(fread(&(level->height), sizeof(short), 1, file) == 1);
	int level_size = level->height * level->width;
	level->on_screen = malloc(sizeof(bool) * level_size);
	level->flat = malloc(sizeof(bool) * level_size * 2);

	level->tiles = malloc(sizeof(float) * level_size * 5);
	int i;
	for(i = 0; i < level_size; i++)
	{
		int j;
		for(j = 0; j < 5; j++)
		{
			assert(fread(&buffer, sizeof(short), 1, file) == 1);
			level->tiles[i][j] = buffer/2.f;
		}
		float tb_avg = (level->tiles[i][T] + level->tiles[i][B])/2.f;
		bool t_equals_b = equalf(level->tiles[i][T], level->tiles[i][B], .1f);
		level->flat[i][0] = t_equals_b && equalf(level->tiles[i][L], tb_avg, .1f);
		level->flat[i][1] = t_equals_b && equalf(level->tiles[i][R], tb_avg, .1f);
	}
	calculate_level_projection(level);

	read_colors(level, file, level_size);

	read_objects(level, file);

	fclose(file);

	return level;
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
void calculate_tile(float *position, int *tile_index, float *tile_position, Level *level)
{
	int posx = floor(position[X] - position[Y] + .5f);
	int posy = floor(position[X] + position[Y] + .5f);

	*tile_index = level->width * (posy - posx) + posx + floor((posy - posx)/2.);

	if(tile_position) {
		float offset = .5 * ((abs(posx) + abs(posy)) % 2 == 0);
		double i;
		tile_position[X] = modf(position[X] + offset, &i);
		tile_position[Y] = modf(position[Y] + offset, &i);
	}
}

void calculate_level_projection(Level *level)
{
	level->projection = malloc(sizeof(float) * level->height * level->width * 4);
	int tile_position[2];
	for(tile_position[Y] = 0; tile_position[Y] < level->height; tile_position[Y]++)
	{
		for(tile_position[X] = 0; tile_position[X] < level->width; tile_position[X]++)
		{
			int i = tile_position[Y] * level->width + tile_position[X];
			level->projection[i][L] = level->tiles[i][L]/2.f - tile_position[Y]/4.f;
			level->projection[i][T] = level->tiles[i][T]/2.f - tile_position[Y]/4.f + .25f;
			level->projection[i][R] = level->tiles[i][R]/2.f - tile_position[Y]/4.f;
			level->projection[i][B] = level->tiles[i][B]/2.f - tile_position[Y]/4.f - .25f;
		}
	}
}

bool colliding_with_level(Level *level, float *position, float max_z, float height, int unconsidered_tile_index)
{
	int tile_index;
	float tile_frac_position[2];
	calculate_tile(position, &tile_index, tile_frac_position, level);
	if(unconsidered_tile_index != tile_index) {
		float *tile = level->tiles[tile_index];
		int tile_frac_position_x_rounded = round(tile_frac_position[X]);
		float tile_side_z = tile[tile_frac_position_x_rounded ? R : L];
		if(level->flat[tile_index][tile_frac_position_x_rounded]) {
			if(tile_side_z > max_z) {
				return true;
			}
		} else {
			float z = calculate_z_on_plane(
				tile_frac_position_x_rounded, .5f, tile_side_z,
				.5f, 0, tile[T],
				.5f, 1, tile[B],
				tile_frac_position[X], tile_frac_position[Y]);
			if(z > max_z + height) {
				return true;
			}
		}
	}
	return false;
}

void delete_level(Level *level)
{
	free(level->tiles);
	free(level->projection);
	free(level->floor_colors);
	free(level->flat);
	free(level->on_screen);
	delete_objectlist(level->objects, level->num_objects);
	free(level);
}
