#include "inc/marble.h"
#include "marble_p.h"

Marble *init_marble(Level *level, unsigned char color[3], float radius, int num_collision_points)
{
	Marble *marble = malloc(sizeof(Marble));
	marble->physics_process = &physics_process_marble;
	marble->type = MARBLE;
	marble->position[X] = 0;
	marble->position[Y] = 0;
	marble->num_collision_points = num_collision_points;
	calculate_tile(marble->position, &(marble->tile_index), marble->tile_frac_position, level);
	float *tile = level->tiles[marble->tile_index];
	marble->position[Z] = (tile[T] + tile[B])/2.f; //assume that x is in middle of tile
	marble->velocity[X] = 0;
	marble->velocity[Y] = 0;
	marble->radius = radius;
	marble->in_air = false;
	memcpy(marble->color, color, sizeof(unsigned char) * 3);
	return marble;
}

void physics_process_marble(Context *context, Object object)
{
	Marble *marble = object.marble;
	Level *level = context->level;
	//calculate marble->velocity
	marble->velocity[X] += context->input[X] * MARBLE_ACCELERATION;
	marble->velocity[Y] += context->input[Y] * MARBLE_ACCELERATION;
	float *tile = level->tiles[marble->tile_index];
	float tb_avg = (tile[T] + tile[B])/2.f;
	if(! marble->in_air) {
		context->can_control = true;
		if(marble->tile_frac_position[X] <= .5f && tile[L] != tb_avg) {
			marble->velocity[X] += (tile[L] - tb_avg) * GRAVITY_ACCELERATION;
		} else if(marble->tile_frac_position[X] > .5f && tile[R] != tb_avg) {
			marble->velocity[X] += (tb_avg - tile[R]) * GRAVITY_ACCELERATION;
		}
		if(tile[T] != tile[B]) {
			marble->velocity[Y] += (tile[T] - tile[B])/2.f * GRAVITY_ACCELERATION;
		}
		marble->velocity[X] -= FRICTION * marble->velocity[X];
		marble->velocity[Y] -= FRICTION * marble->velocity[Y];
	} else {
		context->can_control = false;
		marble->velocity[Z] -= GRAVITY_ACCELERATION;
	}

	float future_position[3];
	future_position[X] = marble->position[X] + marble->velocity[X];
	future_position[Y] = marble->position[Y] + marble->velocity[Y];

	//calculate marble->tile_index and marble->tile_frac_position
	int future_tile_index;
	float future_tile_frac_position[2];
	calculate_tile(future_position, &future_tile_index, future_tile_frac_position, level);
	float *future_tile = level->tiles[future_tile_index];

	//calculate marble->position
	int future_tile_frac_position_x_rounded = round(future_tile_frac_position[X]);
	future_position[Z] = calculate_z_on_plane(
		future_tile_frac_position_x_rounded, .5f, future_tile[future_tile_frac_position_x_rounded ? R : L],
		.5f, 0, future_tile[T],
		.5f, 1, future_tile[B],
		future_tile_frac_position[X], future_tile_frac_position[Y]);

	//calculate collision_positions
	float velocity_angle = atan2(marble->velocity[Y], marble->velocity[X]);
	float angle = velocity_angle + COLLISION_ANGLE_PADDING - M_TAU / 4.f;
	float dangle = (M_TAU - 2 * COLLISION_ANGLE_PADDING) / marble->num_collision_points;
	float collision_positions[marble->num_collision_points][2];
	int i;
	for(i = 0; i < marble->num_collision_points; i++)
	{
		collision_positions[i][X] = future_position[X] + marble->radius * cosf(angle);
		collision_positions[i][Y] = future_position[Y] + marble->radius * sinf(angle);
		angle += dangle;
	}

	//collision with CollisionObjects
	bool collided = false;
	for(i = 0; i < level->num_objects; i++)
	{
		object = level->objects[i];
		switch(object.common->type)
		{
			case COLLISIONAREA: ;
			CollisionArea *collision_area = object.collision_area;
			int j;
			for(j = 0; j < marble->num_collision_points; j++)
			{
				if(in_collision_area(collision_area, collision_positions[j])) {
					collision_area->colliding = true;
					if(!collision_area->can_move_over) {
						collided = true;
					}
				}
			}
		}
	}

	bool future_in_air = marble->position[Z] - future_position[Z] > MAX_DELTA_Z;

	//collision with level
	if(!collided) {
		if(!future_in_air) {

			for(i = 0; i < marble->num_collision_points; i++)
			{
				if(colliding_with_level(level, collision_positions[i], future_position[Z] + MAX_DELTA_Z, marble->radius, future_tile_index)) {
					collided = true;
					break;
				}
			}
		} else {//only check collision with point directly ahead if in air
			float collision_position[2] = {future_position[X] + marble->radius * cosf(velocity_angle),
				future_position[Y] + marble->radius * sinf(velocity_angle)};
			if(colliding_with_level(level, collision_position, future_position[Z] + MAX_DELTA_Z, marble->radius, future_tile_index)) {
				collided = true;
			}
		}
	}

	if(collided) {
		marble->velocity[X] = 0;
		marble->velocity[Y] = 0;
	} else {
		marble->tile_index = future_tile_index;
		marble->tile_frac_position[X] = future_tile_frac_position[X];
		marble->tile_frac_position[Y] = future_tile_frac_position[Y];
		marble->in_air = future_in_air;
		if(!marble->in_air) {
			marble->velocity[Z] = future_position[Z] - marble->position[Z];
		}
		marble->position[X] = future_position[X];
		marble->position[Y] = future_position[Y];
		marble->position[Z] += marble->velocity[Z];
	}

	//scroll screen
	float marble_screen_pos[2] = {marble->position[X] - level->scroll_offset[X],
		marble->position[Z] - marble->position[Y] + level->scroll_offset[Y]};
	if(marble_screen_pos[Y] <= SCROLL_BORDER) {
		level->scroll_offset[Y] = marble->position[Y] - marble->position[Z] + SCROLL_BORDER;
		level->scroll = true;
	}
	if(marble_screen_pos[Y] >= TILES_ON_SCREEN_Y * 2 - SCROLL_BORDER) {
		level->scroll_offset[Y] = marble->position[Y] - marble->position[Z] - SCROLL_BORDER + TILES_ON_SCREEN_Y * 2;
		level->scroll = true;
	}
	if(marble_screen_pos[X] >= TILES_ON_SCREEN_X - SCROLL_BORDER) {
		level->scroll_offset[X] = marble->position[X] + SCROLL_BORDER - TILES_ON_SCREEN_X;
		level->scroll = true;
	}
	if(marble_screen_pos[X] <= SCROLL_BORDER) {
		level->scroll_offset[X] = marble->position[X] - SCROLL_BORDER;
		level->scroll = true;
	}
}
