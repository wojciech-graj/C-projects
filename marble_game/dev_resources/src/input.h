#ifndef INPUT_H
#define INPUT_H

#include <SDL2/SDL.h>

#include "context.h"
#include "sdl_context.h"
#include "menu.h"
#include "math_functions.h"

void game_input_process(SDLContext *sdl_context, Context *context);
void menu_input_process(SDLContext *sdl_context, Context *context, MenuContext *menu_context);
void keybind_input_process(SDLContext *sdl_context, Context *context, MenuContext *menu_context);

#endif
