#include "main.h"

static SDLContext *init_sdl(Config *config)
{
    assert(SDL_Init(SDL_INIT_EVERYTHING) == 0);
	SDLContext *context =  malloc(sizeof(SDLContext));

	assert(SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE) == 0);
	assert(SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) == 0);

	assert(SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16) == 0);

	const int *resolution = RESOLUTIONS[config->resolution_index];
	context->window = SDL_CreateWindow("MARBLE GAME",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		resolution[X], resolution[Y],
		SDL_WINDOW_OPENGL);
	assert(context->window);

	context->main_context = SDL_GL_CreateContext(context->window);
	assert(context->main_context);

	context->keystates = SDL_GetKeyboardState(NULL);
	assert(context->keystates);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glDepthRange(-1, 1);

	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	return context;
}

static void quit(SDLContext *sdl_context, Context *context)
{
	delete_context(context);
	SDL_GL_DeleteContext(sdl_context->main_context);
	SDL_DestroyWindow(sdl_context->window);
	free(sdl_context);
	SDL_Quit();
	exit(0);
}

static void resize(SDLContext *sdl_context, Context *context)
{
	const int *resolution = RESOLUTIONS[context->config->resolution_index];
	SDL_SetWindowSize(sdl_context->window, resolution[X], resolution[Y]);
	glViewport(0, 0, resolution[X], resolution[Y]);
}

int main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	Config *config = load_config("config");
	SDLContext *sdl_context = init_sdl(config);
	Context *context = init_context(config);
	MenuContext *menu_context = init_menu_context(MENU_MAIN);

	load_textures("resources/textures", context);

	load_level("resources/level1", context);

	int i;
	while(true)
	{
		Uint32 frame_start = SDL_GetTicks();

		if(context->resize) {
			context->resize = false;
			resize(sdl_context, context);
		}
		if(context->scroll) {
			context->scroll = false;
			calculate_on_screen(context);
		}

		switch(context->gamestate)
		{
			case STATE_GAME:
			game_input_process(sdl_context, context);
			break;
			case STATE_MENU:
			menu_input_process(sdl_context, context, menu_context);
			break;
			case STATE_KEYBIND:
			keybind_input_process(sdl_context, context, menu_context);
			break;
		}
		if(context->quit) {
			if(menu_context) {
				delete_menu_context(menu_context);
			}
			goto QUIT_GAME;
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		draw_game(context);
		switch(context->gamestate)
		{
			case STATE_GAME:
			for(i = 0; i < context->num_objects; i++)
			{
				if(context->objects[i].common->physics_process) {
					context->objects[i].common->physics_process(context, context->objects[i]);
				}
			}
			break;
			case STATE_MENU:
			draw_menu(context, menu_context);
			if(menu_context->exit == true) {
				delete_menu_context(menu_context);
				menu_context = NULL;
				context->gamestate = STATE_GAME;
			}
			break;
			case STATE_KEYBIND:
			draw_menu(context, menu_context);
			break;
		}

		SDL_GL_SwapWindow(sdl_context->window);

		context->timer++;
		Uint32 frame_time = SDL_GetTicks() - frame_start;
		SDL_Delay(FRAMETIME - frame_time);
		DBG_LOG("FRAMETIME: %d\n", frame_time);
	}

	QUIT_GAME:
	quit(sdl_context, context);
	return 0;
}
