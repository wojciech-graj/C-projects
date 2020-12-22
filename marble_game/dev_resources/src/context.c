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
		.timer = 0,
		.can_control = true,
		.config = config};
	return context;
}

void delete_context(Context *context)
{
	free(context->textures);
	delete_level(context->level);
	free(context->config);
	free(context);
}
