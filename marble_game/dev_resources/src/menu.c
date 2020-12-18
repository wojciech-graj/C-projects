#include "menu.h"

const int MENU_OPTIONS_INDEX = MENU_OPTIONS;

const MenuButton MENU_MAIN_BUTTONS[MENU_MAIN_NUM_BUTTONS] = {
	{{.25f, .25f}, {.5f, .15f}, "START", &on_pressed_exit, NULL},
	{{.25f, .4f}, {.5f, .15f}, "OPTIONS", &on_pressed_change_menu, &MENU_OPTIONS_INDEX},
	{{.25f, .65f}, {.5f, .15f}, "QUIT", &on_pressed_quit, NULL}};
const MenuButton MENU_OPTIONS_BUTTONS[MENU_OPTIONS_NUM_BUTTONS] = {
	{{.25f, .25f}, {.5f, .25f}, "RESOLUTION", NULL, NULL}};

const Menu MENUS[NUM_MENUS] = {
	[MENU_MAIN] = {MENU_MAIN_NUM_BUTTONS, MENU_MAIN_BUTTONS},
	[MENU_OPTIONS] = {MENU_OPTIONS_NUM_BUTTONS, MENU_OPTIONS_BUTTONS}};

void on_pressed_exit(Context *context, MenuContext *menu_context, const void *data)
{
	(void)context;
	(void)data;
	menu_context->exit = true;
}

void on_pressed_quit(Context *context, MenuContext *menu_context, const void *data)
{
	(void)menu_context;
	(void)data;
	context->quit = true;
}

void on_pressed_change_menu(Context *context, MenuContext *menu_context, const void *data)
{
	(void)context;
	menu_context->selected_menu = *((int*) data);
}

void delete_menu_context(MenuContext *menu_context)
{
	free(menu_context);
}

MenuContext *init_menu_context(int menu_index)
{
	MenuContext *menu_context = malloc(sizeof(MenuContext));
	*menu_context = (MenuContext) {
		.menu_context = menu_index,
		.selected_button = 0,
		.selected_menu = 0,
		.exit = false};
	return menu_context;
}
