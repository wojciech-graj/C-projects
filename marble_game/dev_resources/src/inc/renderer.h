#ifndef RENDERER_H
#define RENDERER_H

#include "global.h"

typedef struct Context Context;
typedef struct MenuContext MenuContext;

void calculate_on_screen(Context *context);//TODO:move to level
void draw_game(Context *context);
void draw_menu(Context *context, MenuContext *menu_context);

#endif
