#include "context.h"

Context *init_context(void)
{
	Context *context = malloc(sizeof(Context));
	context->level = NULL;
	context->projection = NULL;
	context->scroll = true;
	context->scroll_offset[X] = 0;
	context->scroll_offset[Y] = 0;
	context->player_marble = NULL;
	context->goal = NULL;
	return context;
}

void delete_context(Context *context)
{
	free(context->level);
	free(context->projection);
	free(context->player_marble);
	free(context->goal);
	free(context);
}
