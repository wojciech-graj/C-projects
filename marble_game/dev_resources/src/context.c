#include "inc/context.h"
#include "context_p.h"

Context *init_context(Config *config)
{
	Context *context = malloc(sizeof(Context));
	*context = (Context) {
		.resolution_index = DEFAULT_RESOLUTION_INDEX,
		.level = NULL,
		.gamestate = STATE_MENU,
		.quit = false,
		.resize = false,
		.projection = NULL,
		.flat = NULL,
		.on_screen = NULL,
		.timer = 0,
		.floor_colors = NULL,
		.can_control = true,
		.scroll = true,
		.scroll_offset[X] = 0,
		.scroll_offset[Y] = 0,
		.objects = NULL,
		.config = config};
	return context;
}

void delete_context(Context *context)
{
	free(context->level);
	free(context->projection);
	free(context->floor_colors);
	free(context->flat);
	free(context->on_screen);
	free(context->textures);
	delete_objectlist(context->objects, context->num_objects);
	free(context->config);
	free(context);
}
