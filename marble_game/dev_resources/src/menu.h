#ifndef MENU_H
#define MENU_H

#include <stdbool.h>
#include <string.h>

#include "global.h"
#include "context.h"
#include "sprite.h"

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

MenuContext *init_menu_context(int menu_context_preset);
Menu *init_menu(int menu_preset);
void delete_menu_context(MenuContext *menu_context);

void on_pressed_exit(Context *context, MenuContext *menu_context, const void *data);
void on_pressed_quit(Context *context, MenuContext *menu_context, const void *data);
void on_pressed_change_menu(Context *context, MenuContext *menu_context, const void *data);

enum menu_presets{MENU_MAIN, MENU_OPTIONS};

#define NUM_MENUS 2
#define MENU_MAIN_NUM_BUTTONS 3
#define MENU_OPTIONS_NUM_BUTTONS 1

static const int REWORK_THIS_PART_OF_THE_CODE = 1;

static const MenuButton MENU_MAIN_BUTTONS[MENU_MAIN_NUM_BUTTONS] = {
	{{.25f, .25f}, {.5f, .15f}, "START", &on_pressed_exit, NULL},
	{{.25f, .4f}, {.5f, .15f}, "OPTIONS", &on_pressed_change_menu, &REWORK_THIS_PART_OF_THE_CODE},
	{{.25f, .65f}, {.5f, .15f}, "QUIT", &on_pressed_quit, NULL}};
static const MenuButton MENU_OPTIONS_MENUS[MENU_OPTIONS_NUM_BUTTONS] = {
	{{.25f, .25f}, {.5f, .25f}, "RESOLUTION", NULL, NULL}};

static const Menu MENUS[NUM_MENUS] = {
	{MENU_MAIN_NUM_BUTTONS, MENU_MAIN_BUTTONS},
	{MENU_OPTIONS_NUM_BUTTONS, MENU_OPTIONS_MENUS}};

#endif
