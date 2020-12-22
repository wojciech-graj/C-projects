#ifndef RENDERER_H
#define RENDERER_H

#include "global.h"

typedef struct Context Context;
typedef struct MenuContext MenuContext;
typedef struct Level Level;

void draw_game(Context *context);
void draw_menu(Context *context, MenuContext *menu_context);
void calculate_on_screen(Level *level);

#endif
