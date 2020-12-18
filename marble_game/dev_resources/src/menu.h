#ifndef MENU_H
#define MENU_H

#include <stdbool.h>
#include <string.h>

#include "global.h"
#include "context.h"
#include "sprite.h"

typedef struct Menu Menu;
typedef struct MenuButton MenuButton;

typedef struct MenuContext {
	int selected_button;
	int selected_menu;
	bool exit;
	int num_menus;
	Menu **menus;
} MenuContext;

typedef struct Menu {
	int num_sprites;
	int num_buttons;
	Sprite **sprites;
	MenuButton **buttons;
} Menu;

typedef struct MenuButton {
	void (*on_selected)(MenuContext*);
	void (*on_deselected)(MenuContext*);
	void (*on_pressed)(MenuContext*);
	float position[2]; //TL corner
	float size[2];
	int text_length;
	char *text;
	Sprite *background;
} MenuButton;

MenuContext *init_menu_context(int menu_context_preset);
Menu *init_menu(int menu_preset);
void delete_menu_context(MenuContext *menu_context);

#endif
