#ifndef MENU_H
#define MENU_H

#include <stdbool.h>

#include "global.h"

#define NUM_MENUS 3

enum menu_presets{MENU_MAIN, MENU_OPTIONS, MENU_KEYBINDS};

typedef struct Context Context;

typedef struct MenuContext MenuContext;
typedef struct Menu Menu;
typedef struct MenuButton MenuButton;
typedef struct MenuDynamicText MenuDynamicText;

typedef struct MenuContext {
	int menu_context;
	int selected_button;
	int selected_menu;
	bool exit;
	int rebound_input;
} MenuContext;

typedef struct Menu {
	void (*on_escape)(Context*, MenuContext*, const void*, int);
	const void *data;
	const int num_buttons;
	const MenuButton *buttons;
	const int num_dynamic_texts;
	const MenuDynamicText *dynamic_texts;
} Menu;

typedef struct MenuButton {
	const float position[2];
	const float size[2];
	const char *text;
	void (*on_pressed)(Context*, MenuContext*, const void*, int);
	const void *data;
} MenuButton;

typedef struct MenuDynamicText {
	const float position[2];
	const float size[2];
	const int text_type;
	const void *data;
} MenuDynamicText;

MenuContext *init_menu_context(int menu_index);
void delete_menu_context(MenuContext *menu_context);

char *get_dynamic_text(Context *context, MenuContext *menu_context, const MenuDynamicText dynamic_text);

extern const Menu MENUS[NUM_MENUS];

#endif
