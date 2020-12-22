#ifndef INPUT_H
#define INPUT_H

typedef struct Context Context;
typedef struct SDLContext SDLContext;
typedef struct MenuContext MenuContext;

void game_input_process(SDLContext *sdl_context, Context *context);
void menu_input_process(SDLContext *sdl_context, Context *context, MenuContext *menu_context);
void keybind_input_process(SDLContext *sdl_context, Context *context, MenuContext *menu_context);

#endif
