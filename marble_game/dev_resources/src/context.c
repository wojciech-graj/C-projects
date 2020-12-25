#include "inc/context.h"
#include "context_p.h"

Context *init_context(Config *config)
{
	Context *context = malloc(sizeof(Context));
	*context = (Context) {
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
	delete_sound(context->sound);
	free(context->config);
	free(context);
}
