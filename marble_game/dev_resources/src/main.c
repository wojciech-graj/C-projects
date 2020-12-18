#include "main.h"

SDLContext *init_sdl(void)
{
    assert(SDL_Init(SDL_INIT_EVERYTHING) == 0);
	SDLContext *context =  malloc(sizeof(SDLContext));

	assert(SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE) == 0);
	assert(SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) == 0);

	assert(SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16) == 0);

	int *resolution = RESOLUTIONS[DEFAULT_RESOLUTION_INDEX];
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

void quit(SDLContext *sdl_context, Context *context)
{
	delete_context(context);
	SDL_GL_DeleteContext(sdl_context->main_context);
	SDL_DestroyWindow(sdl_context->window);
	free(sdl_context);
	SDL_Quit();
	exit(0);
}

static void game_input_process(SDLContext *sdl_context, Context *context)
{
	SDL_Event event;

	while (SDL_PollEvent(&event) > 0)
	{
		if (event.type == SDL_QUIT) {
			context->quit = true;
		}
	}
	context->input[X] = 0;
	context->input[Y] = 0;
	if(context->can_control) {
		if(sdl_context->keystates[SDL_SCANCODE_LEFT]) {
			context->input[X] -= 1.f;
	    }
	    if(sdl_context->keystates[SDL_SCANCODE_RIGHT]) {
			context->input[X] += 1.f;
	    }
	    if(sdl_context->keystates[SDL_SCANCODE_UP]) {
			context->input[Y] -= 1.f;
	    }
	    if(sdl_context->keystates[SDL_SCANCODE_DOWN]) {
			context->input[Y] += 1.f;
	    }
	}
}

static void resize(SDLContext *sdl_context, Context *context)
{
	context->resize = false;
	int *resolution = RESOLUTIONS[context->resolution_index];
	SDL_SetWindowSize(sdl_context->window, resolution[X], resolution[Y]);
}

static void menu_input_process(SDLContext *sdl_context, Context *context, MenuContext *menu_context)
{
	(void) sdl_context;
	SDL_Event event;

	while (SDL_PollEvent(&event) > 0)
	{
		switch(event.type)
		{
			case SDL_QUIT:
			context->quit = true;
			break;
			case SDL_KEYDOWN: ;
			Menu menu = MENUS[menu_context->selected_menu];
			switch(event.key.keysym.sym)
			{
				case SDLK_RETURN: ;
				MenuButton menu_button = menu.buttons[menu_context->selected_button];
				menu_button.function(context, menu_context, menu_button.data);
				break;
				case SDLK_UP:
				menu_context->selected_button = (menu_context->selected_button - 1) % menu.num_buttons;
				break;
				case SDLK_DOWN:
				menu_context->selected_button = (menu_context->selected_button + 1) % menu.num_buttons;
				break;
			}
			break;
		}
	}
}

int main(int argc, char *argv[])
{
	(void)argc;
	(void)argv;
	SDLContext *sdl_context = init_sdl();
	Context *context = init_context();
	MenuContext *menu_context = init_menu_context(MENU_MAIN);

	load_textures("resources/textures", context);

	load_level("resources/level1", context);

	glLoadIdentity();
	gluOrtho2D(0.f, 1.f, 1.f, 0.f); //menu layout

	int i;
	while(true)
	{
		Uint32 frame_start = SDL_GetTicks();

		switch(context->gamestate)
		{
			case GAME:
			game_input_process(sdl_context, context);
			if(context->quit) {
				goto QUIT_GAME;
			}
			for(i = 0; i < context->num_objects; i++)
			{
				if(context->objects[i].common->physics_process) {
					context->objects[i].common->physics_process(context, context->objects[i]);
				}
			}
			draw_game(sdl_context, context);
			break;
			case MENU:
			menu_input_process(sdl_context, context, menu_context);
			if(context->quit) {
				delete_menu_context(menu_context);
				goto QUIT_GAME;
			}
			if(menu_context->exit == true) {
				delete_menu_context(menu_context);
				context->gamestate = GAME;
			} else {
				draw_menu(sdl_context, context, menu_context);
			}
			break;
		}

		if(context->resize) {
			resize(sdl_context, context);
		}

		context->timer++;
		Uint32 frame_time = SDL_GetTicks() - frame_start;
		SDL_Delay(FRAMETIME - frame_time);
		DBG_LOG("FRAMETIME: %d\n", frame_time);
	}

	QUIT_GAME:
	quit(sdl_context, context);
	return 0;
}
