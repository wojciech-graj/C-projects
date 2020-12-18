#ifndef MENU_H
#define MENU_H

#include <stdbool.h>
#include <string.h>

#include "global.h"
#include "context.h"

#define NUM_MENUS 2
#define MENU_MAIN_NUM_BUTTONS 3
#define MENU_OPTIONS_NUM_BUTTONS 1

enum menu_presets{MENU_MAIN, MENU_OPTIONS};

typedef struct MenuContext MenuContext;
typedef struct Menu Menu;
typedef struct MenuButton MenuButton;

typedef struct MenuContext {
	int menu_context;
	int selected_button;
	int selected_menu;
	bool exit;
} MenuContext;

typedef struct MenuButton {
	const float position[2];
	const float size[2];
	const char *text;
	void (*function)(Context*, MenuContext*, const void*);
	const void *data;
} MenuButton;

typedef struct Menu {
	const int num_buttons;
	const MenuButton *buttons;
} Menu;

MenuContext *init_menu_context(int menu_index);
void delete_menu_context(MenuContext *menu_context);

void on_pressed_exit(Context *context, MenuContext *menu_context, const void *data);
void on_pressed_quit(Context *context, MenuContext *menu_context, const void *data);
void on_pressed_change_menu(Context *context, MenuContext *menu_context, const void *data);

extern const Menu MENUS[NUM_MENUS];

#endif
