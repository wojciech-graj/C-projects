#include "inc/area.h"
#include "area_p.h"

Area *init_area(Context *context, void (*physics_process)(Context*, Object), Sprite *sprite, CollisionArea *collision_area, short tile_positions[4][2])
{
	Area *area = malloc(sizeof(Area));
	area->physics_process = physics_process;
	area->type = AREA;
	area->sprite = sprite;
	area->collision_area = collision_area;

	int i;
	for(i = 0; i < 4; i++)
	{
		int offset = tile_positions[i][Y] % 2;
		area->corner_positions[i][X] = tile_positions[i][X] + offset/2.f + ((i - 1) % 2)/2.f;
		area->corner_positions[i][Y] = tile_positions[i][Y]/2.f + ((i - 2) % 2)/2.f;
		area->corner_tile_indexes[i] = tile_positions[i][Y] * context->width + tile_positions[i][X];
	}

	area->side_lengths[X] = round(distance(area->corner_positions[L], area->corner_positions[T]) * (float) sqrt(2));
	area->side_lengths[Y] = round(distance(area->corner_positions[T], area->corner_positions[R]) * (float) sqrt(2));

	area->tile_side_lengths[X] = 1.f / area->side_lengths[X];
	area->tile_side_lengths[Y] = 1.f / area->side_lengths[Y];

	return area;
}
