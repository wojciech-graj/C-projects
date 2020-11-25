#include "context.h"

Context *init_context(void)
{
	Context *context = malloc(sizeof(Context));
	context->level = NULL;
	context->projection = NULL;
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
	delete_objectlist(context->objects, NUM_OBJECTS);
	free(context);
}
