#include "inc/collision_area.h"
#include "collision_area_p.h"

CollisionArea *init_collision_area(void (*physics_process)(Context*, Object), float corner_positions[4][2], bool can_move_over)
{
	CollisionArea *collision_area = malloc(sizeof(CollisionArea));
	collision_area->physics_process = physics_process;
	collision_area->type = COLLISIONAREA;
	collision_area->colliding = false;
	collision_area->can_move_over = can_move_over;

	memcpy(collision_area->corner_positions, corner_positions, sizeof(float) * 8);

	collision_area->vectors[0][X] = collision_area->corner_positions[T][X] - collision_area->corner_positions[L][X];
	collision_area->vectors[0][Y] = collision_area->corner_positions[T][Y] - collision_area->corner_positions[L][Y];
	collision_area->vectors[1][X] = collision_area->corner_positions[R][X] - collision_area->corner_positions[T][X];
	collision_area->vectors[1][Y] = collision_area->corner_positions[R][Y] - collision_area->corner_positions[T][Y];

	collision_area->dot_products[0] = dot_product(collision_area->vectors[0], collision_area->vectors[0]);
	collision_area->dot_products[1] = dot_product(collision_area->vectors[1], collision_area->vectors[1]);
	return collision_area;
}

bool in_collision_area(CollisionArea *collision_area, float *position)
{
	float vec_lp[2] = {position[X] - collision_area->corner_positions[L][X],
		position[Y] - collision_area->corner_positions[L][Y]};
	float vec_tp[2] = {position[X] - collision_area->corner_positions[T][X],
		position[Y] - collision_area->corner_positions[T][Y]};
	float dot_ltlp = dot_product(collision_area->vectors[0], vec_lp);
	float dot_trtp = dot_product(collision_area->vectors[1], vec_tp);
	return (0 <= dot_ltlp && dot_ltlp <= collision_area->dot_products[0] && 0 <= dot_trtp && dot_trtp <= collision_area->dot_products[1]);
}
