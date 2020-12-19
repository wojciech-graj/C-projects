#include "input.h"

void game_input_process(SDLContext *sdl_context, Context *context)
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
		if(sdl_context->keystates[INPUT_SCANCODES[I_LEFT]]) {
			context->input[X] -= 1.f;
	    }
	    if(sdl_context->keystates[INPUT_SCANCODES[I_RIGHT]]) {
			context->input[X] += 1.f;
	    }
	    if(sdl_context->keystates[INPUT_SCANCODES[I_UP]]) {
			context->input[Y] -= 1.f;
	    }
	    if(sdl_context->keystates[INPUT_SCANCODES[I_DOWN]]) {
			context->input[Y] += 1.f;
	    }
	}
}

void menu_input_process(SDLContext *sdl_context, Context *context, MenuContext *menu_context)
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
			SDL_Keycode key = event.key.keysym.sym;
			if(key == INPUT_KEYCODES[I_RETURN]) {
				MenuButton menu_button = menu.buttons[menu_context->selected_button];
				menu_button.on_pressed(context, menu_context, menu_button.data, I_RETURN);
			} else if(key == INPUT_KEYCODES[I_LEFT]) {
				MenuButton menu_button = menu.buttons[menu_context->selected_button];
				menu_button.on_pressed(context, menu_context, menu_button.data, I_LEFT);
			} else if(key == INPUT_KEYCODES[I_RIGHT]) {
				MenuButton menu_button = menu.buttons[menu_context->selected_button];
				menu_button.on_pressed(context, menu_context, menu_button.data, I_RIGHT);
			} else if(key == INPUT_KEYCODES[I_UP]) {
				menu_context->selected_button = (menu_context->selected_button - 1) % menu.num_buttons;
			} else if(key == INPUT_KEYCODES[I_DOWN]) {
				menu_context->selected_button = (menu_context->selected_button + 1) % menu.num_buttons;
			} else if(key == INPUT_KEYCODES[I_ESCAPE]) {
				menu.on_escape(context, menu_context, menu.data, I_ESCAPE);
			}
			break;
		}
	}
}
