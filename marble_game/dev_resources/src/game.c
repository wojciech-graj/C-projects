#include "game.h"

SDL_Context *init_sdl(void)
{
    assert(SDL_Init(SDL_INIT_EVERYTHING) == 0);
	SDL_Context *context =  malloc(sizeof(SDL_Context));

	context->window = SDL_CreateWindow("MARBLE GAME",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH, WINDOW_HEIGHT,
		SDL_WINDOW_OPENGL);
	assert(context->window);

	context->main_context = SDL_GL_CreateContext(context->window);
	assert(context->main_context);

	context->keystates = SDL_GetKeyboardState(NULL);
	assert(context->keystates);

	assert(SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE) == 0);
	assert(SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) == 0);

	glEnable(GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	gluOrtho2D(0, TILES_ON_SCREEN, 0, TILES_ON_SCREEN);
	return context;
}

void quit(SDL_Context *sdl_context, Context *context)
{
	delete_context(context);
	SDL_GL_DeleteContext(sdl_context->main_context);
	SDL_DestroyWindow(sdl_context->window);
	free(sdl_context);
	SDL_Quit();
	exit(0);
}

int input_process(SDL_Context *sdl_context, Context *context)
{
	SDL_Event event;

	while (SDL_PollEvent(&event) > 0)
	{
		if (event.type == SDL_QUIT) {
			return 1;
		}
	}
	if(context->can_control) {
		if(sdl_context->keystates[SDL_SCANCODE_LEFT]) {
			context->objects[ID_PLAYER_MARBLE].marble->velocity[X] -= MARBLE_ACCELERATION;
	    }
	    if(sdl_context->keystates[SDL_SCANCODE_RIGHT]) {
			context->objects[ID_PLAYER_MARBLE].marble->velocity[X] += MARBLE_ACCELERATION;
	    }
	    if(sdl_context->keystates[SDL_SCANCODE_UP]) {
			context->objects[ID_PLAYER_MARBLE].marble->velocity[Y] -= MARBLE_ACCELERATION;
	    }
	    if(sdl_context->keystates[SDL_SCANCODE_DOWN]) {
			context->objects[ID_PLAYER_MARBLE].marble->velocity[Y] += MARBLE_ACCELERATION;
	    }
	}
	return 0;
}

int main(int argc, char *argv[])
{
	SDL_Context *sdl_context = init_sdl();
	Context *context = init_context();

	load_level("resources/level1", context);
	load_textures("resources/textures", context);

	while(true)
	{
		Uint32 frame_start = SDL_GetTicks();

		if(input_process(sdl_context, context) == 1) break;

		int i;
		for(i = 0; i < NUM_OBJECTS; i++)
		{
			if(context->objects[i].common->physics_process) {
				context->objects[i].common->physics_process(context, &(context->objects[i]));
			}
		}

		draw(sdl_context, context);

		Uint32 frame_time = SDL_GetTicks() - frame_start;
		SDL_Delay(FRAMETIME - frame_time);
	}

	quit(sdl_context, context);
	return 0;
}
