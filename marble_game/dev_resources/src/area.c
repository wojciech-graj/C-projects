#include "area.h"

Area *init_area(int texture_index, short tile_positions[4][2], short level_width)
{
	Area *area = malloc(sizeof(Area));
	area->texture_index = texture_index;
	area->physics_process = NULL;
	int i;
	for(i = 0; i < 4; i++)
	{
		int offset = tile_positions[i][Y] % 2;
		area->positions[i][X] = tile_positions[i][X] + offset/2.f + ((i - 1) % 2)/2.f;
		area->positions[i][Y] = tile_positions[i][Y]/2.f + ((i - 2) % 2)/2.f;
		area->tile_indexes[i] = tile_positions[i][Y] * level_width + tile_positions[i][X];
	}
	return area;
}

bool area_colliding(Area *area, float *position)
{
	float vec_lt[2] = {area->positions[T][X] - area->positions[L][X],
		area->positions[T][Y] - area->positions[L][Y]};
	float vec_lp[2] = {position[X] - area->positions[L][X],
		position[Y] - area->positions[L][Y]};
	float vec_tr[2] = {area->positions[R][X] - area->positions[T][X],
		area->positions[R][Y] - area->positions[T][Y]};
	float vec_tp[2] = {position[X] - area->positions[T][X],
		position[Y] - area->positions[T][Y]};
	float dot_ltlp = dot_product(vec_lt, vec_lp);
	float dot_ltlt = dot_product(vec_lt, vec_lt);
	float dot_trtp = dot_product(vec_tr, vec_tp);
	float dot_trtr = dot_product(vec_tr, vec_tr);
	return(0 <= dot_ltlp && dot_ltlp <= dot_ltlt && 0 <= dot_trtp && dot_trtp <= dot_trtr);
}
