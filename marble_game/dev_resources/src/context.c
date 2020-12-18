#include "context.h"

Context *init_context(void)
{
	Context *context = malloc(sizeof(Context));
	context->resolution_index = DEFAULT_RESOLUTION_INDEX;
	context->level = NULL;
	context->gamestate = MENU;
	context->quit = false;
	context->resize = false;
	context->projection = NULL;
	context->flat = NULL;
	context->on_screen = NULL;
	context->timer = 0;
	context->floor_colors = NULL;
	context->can_control = true;
	context->scroll = true;
	context->scroll_offset[X] = 0;
	context->scroll_offset[Y] = 0;
	context->floor_colors = NULL;
	context->objects = NULL;
	return context;
}

void delete_context(Context *context)
{
	free(context->level);
	free(context->projection);
	free(context->floor_colors);
	free(context->flat);
	free(context->on_screen);
	delete_objectlist(context->objects, context->num_objects);
	free(context);
}
