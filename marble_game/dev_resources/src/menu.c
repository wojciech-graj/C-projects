#include "menu.h"
#include "menu_config.h"

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

MenuContext *init_menu_context(int menu_context_preset)
{
	MenuContext *menu_context = malloc(sizeof(MenuContext));
	menu_context->menu_context = menu_context_preset;
	menu_context->selected_button = 0;
	menu_context->selected_menu = 0;
	menu_context->exit = false;
	return menu_context;
}
