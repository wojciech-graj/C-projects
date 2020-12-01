#include "sprite.h"

Sprite *init_sprite(void (*physics_process)(Context*, Object), float corner_projections[4][2], int texture_index, int frame_time, bool rotate, bool flip_x, bool flip_y)
{
	Sprite *sprite = malloc(sizeof(Sprite));
	sprite->physics_process = physics_process;
	sprite->type = SPRITE;
	sprite->texture_index = texture_index;
	sprite->frame_time = frame_time;
	sprite->frame = 0;
	sprite->rotate = rotate;
	sprite->flip_x = flip_x;
	sprite->flip_y = flip_y;
	memcpy(sprite->corner_projections, corner_projections, sizeof(float) * 8);
	return sprite;
}

void physics_process_animated_sprite(Context *context, Object object)
{
	Sprite *sprite = object.sprite;
	if(context->timer % sprite->frame_time == 0) {
		 sprite->frame = (sprite->frame + 1) % num_texture_frames[sprite->texture_index];
	}
}
