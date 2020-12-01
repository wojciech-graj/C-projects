#include "marble.h"

Marble *init_marble(Context *context, unsigned char color[3])
{
	Marble *marble = malloc(sizeof(Marble));
	marble->physics_process = &physics_process_marble;
	marble->type = MARBLE;
	marble->position[X] = 0;
	marble->position[Y] = 0;
	calculate_tile(marble->position, &(marble->tile_index), marble->tile_frac_position, context);
	float *tile = context->level[marble->tile_index];
	marble->position[Z] = (tile[T] + tile[B])/2.f; //assume that x is in middle of tile
	marble->velocity[X] = 0;
	marble->velocity[Y] = 0;
	marble->radius = .2;
	marble->in_air = false;
	memcpy(marble->color, color, sizeof(unsigned char) * 3);
	return marble;
}

void physics_process_marble(Context *context, Object object)
{
	Marble *marble = object.marble;
	//calculate marble->velocity
	float *tile = context->level[marble->tile_index];
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

	int i;
	for(i = 0; i < NUM_OBJECTS; i++)
	{
		object = context->objects[i];
		switch(object.common->type)
		{
			case COLLISIONAREA: ;
			CollisionArea *collision_area = object.collision_area;
			if(in_collision_area(collision_area, future_position)) {
				goto COLLIDED;
			}
		}
	}

	//calculate marble->tile_index and marble->tile_frac_position
	int future_tile_index;
	float future_tile_frac_position[2];
	calculate_tile(future_position, &future_tile_index, future_tile_frac_position, context);

	float *future_tile = context->level[future_tile_index];
	tb_avg = (future_tile[T] + future_tile[B])/2.f;

	//calculate marble->position and collision
	future_position[Z] = calculate_z_on_plane(
		round(future_tile_frac_position[X]), .5f, future_tile[(future_tile_frac_position[X] < .5f) ? L : R],
		.5f, 0, future_tile[T],
		.5f, 1, future_tile[B],
		future_tile_frac_position[X], future_tile_frac_position[Y]);
	if(future_position[Z] - marble->position[Z] > MAX_DELTA_Z) {
		COLLIDED:
		marble->velocity[X] = 0;
		marble->velocity[Y] = 0;
	} else {
		marble->tile_index = future_tile_index;
		marble->tile_frac_position[X] = future_tile_frac_position[X];
		marble->tile_frac_position[Y] = future_tile_frac_position[Y];
		marble->position[X] = future_position[X];
		marble->position[Y] = future_position[Y];
		if(marble->position[Z] - future_position[Z] > MAX_DELTA_Z) {
			if(!marble->in_air) marble->in_air = true;
		} else {
			if(marble->in_air) marble->in_air = false;
			marble->velocity[Z] = future_position[Z] - marble->position[Z];
		}
		marble->position[Z] += marble->velocity[Z];
	}

	//scroll screen
	float marble_screen_pos[2] = {marble->position[X] - context->scroll_offset[X],
		marble->position[Z] - marble->position[Y] + context->scroll_offset[Y]};
	if(marble_screen_pos[Y] <= SCROLL_BORDER) {
		context->scroll_offset[Y] = marble->position[Y] - marble->position[Z] + SCROLL_BORDER;
		context->scroll = true;
	}
	if(marble_screen_pos[Y] >= TILES_ON_SCREEN * 2 - SCROLL_BORDER) {
		context->scroll_offset[Y] = marble->position[Y] - marble->position[Z] - SCROLL_BORDER + TILES_ON_SCREEN * 2;
		context->scroll = true;
	}
	if(marble_screen_pos[X] >= TILES_ON_SCREEN - SCROLL_BORDER) {
		context->scroll_offset[X] = marble->position[X] + SCROLL_BORDER - TILES_ON_SCREEN;
		context->scroll = true;
	}
	if(marble_screen_pos[X] <= SCROLL_BORDER) {
		context->scroll_offset[X] = marble->position[X] - SCROLL_BORDER;
		context->scroll = true;
	}
}
