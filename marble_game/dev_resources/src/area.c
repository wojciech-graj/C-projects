#include "area.h"
#include <stdio.h>

Area *init_area(Context *context, int texture_index, short tile_positions[4][2], bool rotate, bool flip_x, bool flip_y)
{
	Area *area = malloc(sizeof(Area));
	area->texture_index = texture_index;
	area->physics_process = NULL;
	area->rotate = rotate;
	area->flip_x = flip_x;
	area->flip_y = flip_y;
	int i;
	for(i = 0; i < 4; i++)
	{
		int offset = tile_positions[i][Y] % 2;
		area->corner_positions[i][X] = tile_positions[i][X] + offset/2.f + ((i - 1) % 2)/2.f;
		area->corner_positions[i][Y] = tile_positions[i][Y]/2.f + ((i - 2) % 2)/2.f;

		int tile_index = context->width * tile_positions[i][Y] + tile_positions[i][X];
		area->corner_projections[i] = context->projection[tile_index][i];
	}

	float side_lengths[2] = {round(distance(area->corner_positions[L], area->corner_positions[T]) * (float) sqrt(2)),
	round(distance(area->corner_positions[T], area->corner_positions[R]) * (float) sqrt(2))};

	area->side_lengths_sqr[X] = isqr(side_lengths[X]);
	area->side_lengths_sqr[Y] = isqr(side_lengths[Y]);

	area->half_tile_side_lengths[X] = .5f / side_lengths[X];
	area->half_tile_side_lengths[Y] = .5f / side_lengths[Y];

	area->vectors[0][X] = area->corner_positions[T][X] - area->corner_positions[L][X];
	area->vectors[0][Y] = area->corner_positions[T][Y] - area->corner_positions[L][Y];
	area->vectors[1][X] = area->corner_positions[R][X] - area->corner_positions[T][X];
	area->vectors[1][Y] = area->corner_positions[R][Y] - area->corner_positions[T][Y];

	area->dot_products[0] = dot_product(area->vectors[0], area->vectors[0]);
	area->dot_products[1] = dot_product(area->vectors[1], area->vectors[1]);

	return area;
}

bool in_area(Area *area, float *position, float *area_position)
{
	float vec_lp[2] = {position[X] - area->corner_positions[L][X],
		position[Y] - area->corner_positions[L][Y]};
	float vec_tp[2] = {position[X] - area->corner_positions[T][X],
		position[Y] - area->corner_positions[T][Y]};
	float dot_ltlp = dot_product(area->vectors[0], vec_lp);
	float dot_trtp = dot_product(area->vectors[1], vec_tp);
	bool inside = 0 <= dot_ltlp && dot_ltlp <= area->dot_products[0] && 0 <= dot_trtp && dot_trtp <= area->dot_products[1];
	if(inside && area_position) {
		area_position[X] = 2 * dot_ltlp / area->side_lengths_sqr[X];
		area_position[Y] = 2 * dot_trtp / area->side_lengths_sqr[Y];
	}
	return inside;
}
