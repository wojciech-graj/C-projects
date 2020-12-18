#include "point.h"

Point *init_point(void (*physics_process)(Context*, Object), Sprite *sprite, int tile_index, float z)
{
	Point *point = malloc(sizeof(Point));
	*point = (Point) {
		.physics_process = physics_process,
		.type = POINT,
		.sprite = sprite,
		.tile_index = tile_index,
		.z = z};
	return point;
}

void physics_process_point(Context *context, Object object)
{
	Point *point = object.point;
	if(point->sprite) {
		if(point->sprite->physics_process) {
			Object child_sprite;
			child_sprite.sprite = point->sprite;
			point->sprite->physics_process(context, child_sprite);
		}
	}
}
