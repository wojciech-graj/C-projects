#include "input.h"

void game_input_process(SDLContext *sdl_context, Context *context)
{
	Config *config = context->config;
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
		if(sdl_context->keystates[config->input_scancodes[I_LEFT]]) {
			context->input[X] -= 1.f;
	    }
	    if(sdl_context->keystates[config->input_scancodes[I_RIGHT]]) {
			context->input[X] += 1.f;
	    }
	    if(sdl_context->keystates[config->input_scancodes[I_UP]]) {
			context->input[Y] -= 1.f;
	    }
	    if(sdl_context->keystates[config->input_scancodes[I_DOWN]]) {
			context->input[Y] += 1.f;
	    }
	}
}

void keybind_input_process(SDLContext *sdl_context, Context *context, MenuContext *menu_context)
{
	(void) sdl_context;
	Config *config = context->config;
	SDL_Event event;

	while (SDL_PollEvent(&event) > 0)
	{
		switch(event.type)
		{
			case SDL_QUIT:
			context->quit = true;
			break;
			case SDL_KEYDOWN: ;
			SDL_Keycode key = event.key.keysym.sym;
			config->input_keycodes[menu_context->rebound_input] = key;
			config->input_scancodes[menu_context->rebound_input] = SDL_GetScancodeFromKey(key);
			context->gamestate = STATE_MENU;
			break;
		}
	}
}

void menu_input_process(SDLContext *sdl_context, Context *context, MenuContext *menu_context)
{
	(void) sdl_context;
	Config *config = context->config;
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
			SDL_Keycode key = event.key.keysym.sym;
			if(key == config->input_keycodes[I_RETURN]) {
				MenuButton menu_button = menu.buttons[menu_context->selected_button];
				menu_button.on_pressed(context, menu_context, menu_button.data, I_RETURN);
			} else if(key == config->input_keycodes[I_LEFT]) {
				MenuButton menu_button = menu.buttons[menu_context->selected_button];
				menu_button.on_pressed(context, menu_context, menu_button.data, I_LEFT);
			} else if(key == config->input_keycodes[I_RIGHT]) {
				MenuButton menu_button = menu.buttons[menu_context->selected_button];
				menu_button.on_pressed(context, menu_context, menu_button.data, I_RIGHT);
			} else if(key == config->input_keycodes[I_UP]) {
				menu_context->selected_button = eucmod(--menu_context->selected_button, menu.num_buttons);
			} else if(key == config->input_keycodes[I_DOWN]) {
				menu_context->selected_button = (menu_context->selected_button + 1) % menu.num_buttons;
			} else if(key == config->input_keycodes[I_ESCAPE]) {
				menu.on_escape(context, menu_context, menu.data, I_ESCAPE);
			}
			break;
		}
	}
}
