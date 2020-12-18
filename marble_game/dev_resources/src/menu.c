#include "menu.h"

static MenuButton *init_menu_button(void (*on_pressed)(MenuContext*), Sprite *background, float position[2], float size[2], const char *string)
{
	MenuButton *menu_button = malloc(sizeof(MenuButton));
	menu_button->on_pressed = on_pressed;
	menu_button->background = background;
	menu_button->text_length = strlen(string);
	menu_button->text = malloc(sizeof(char) * (menu_button->text_length + 1));
	memcpy(menu_button->text, string, menu_button->text_length + 1);
	memcpy(menu_button->position, position, sizeof(float) * 2);
	memcpy(menu_button->size, size, sizeof(float) * 2);
	return menu_button;
}

static void on_pressed_start(MenuContext *menu_context)
{
	menu_context->exit = true;
}

//TODO: use lists of menu data (maybe?)
void delete_menu_context(MenuContext *menu_context)
{
	int i;
	for(i = 0; i < menu_context->num_menus; i++)
	{
		Menu *menu = menu_context->menus[i];
		int j;
		for(j = 0; j < menu->num_sprites; j++)
		{
			free(menu->sprites[j]);
		}
		free(menu->sprites);
		for(j = 0; j < menu->num_buttons; j++)
		{
			MenuButton *menu_button = menu->buttons[j];
			free(menu_button->background);
			free(menu_button->text);
			free(menu_button);
		}
		free(menu->buttons);
		free(menu);
	}
	free(menu_context->menus);
	free(menu_context);
}

MenuContext *init_menu_context(int menu_context_preset)
{
	MenuContext *menu_context = malloc(sizeof(MenuContext));
	menu_context->selected_button = 0;
	menu_context->selected_menu = 0;
	menu_context->exit = false;
	switch(menu_context_preset)
	{
		case MENU_CONTEXT_MAIN:
		menu_context->num_menus = 1;
		menu_context->menus = malloc(sizeof(Menu*) * 1);
		menu_context->menus[0] = init_menu(MENU_MAIN);
		break;
	}
	return menu_context;
}

Menu *init_menu(int menu_preset)
{
	Menu *menu = malloc(sizeof(Menu));
	switch(menu_preset)
	{
		case MENU_MAIN:
		menu->num_sprites = 0;
		menu->num_buttons = 1;
		menu->sprites = NULL;
		menu->buttons = malloc(sizeof(MenuButton*) * 1);
		float position[2] = {0.25f, 0.25f};
		float size[2] = {.5f, .5f};
		menu->buttons[0] = init_menu_button(&on_pressed_start, NULL, position, size, "START");
		break;
	}
	return menu;
}
