#include "point.h"

Point *init_point(void (*physics_process)(Context*, Object), Sprite *sprite, int tile_index)
{
	Point *point = malloc(sizeof(Point));
	point->physics_process = physics_process;
	point->type = POINT;
	point->delete = &delete_point;
	point->sprite = sprite;
	point->tile_index = tile_index;
	return point;
}

void physics_process_point(Context *context, Object object)
{
	Point *point = object.point;
	if(point->sprite) {
		if(point->sprite->physics_process) {
			Object child_sprite;
			child_sprite.sprite = point->sprite;
			point->sprite->physics_process(context, &child_sprite);
		}
	}
}

void delete_point(Object object)
{
	Point *point = object.point;
	free(point->sprite);
	free(point);
}
