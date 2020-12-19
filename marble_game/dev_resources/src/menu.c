#include "menu.h"

const int DYNAMIC_TEXT_TYPE_RESOLUTION = TEXT_RESOLUTION;

const int MENU_MAIN_INDEX = MENU_MAIN;
const int MENU_OPTIONS_INDEX = MENU_OPTIONS;

const ContextIncrementData MENU_BUTTON_RESOLUTION_DATA = {offsetof(Context, resolution_index), offsetof(Context, resize), 0, NUM_RESOLUTIONS - 1};

const MenuButton MENU_MAIN_BUTTONS[MENU_MAIN_NUM_BUTTONS] = {
	{{.25f, .25f}, {.5f, .15f}, "PLAY", &on_pressed_exit, NULL},
	{{.25f, .4f}, {.5f, .15f}, "OPTIONS", &on_pressed_change_menu, &MENU_OPTIONS_INDEX},
	{{.25f, .55f}, {.5f, .15f}, "QUIT", &on_pressed_quit, NULL}};
const MenuButton MENU_OPTIONS_BUTTONS[MENU_OPTIONS_NUM_BUTTONS] = {
	{{.25f, .25f}, {.5f, .25f}, "RESOLUTION", &on_pressed_increment, &MENU_BUTTON_RESOLUTION_DATA}};

const MenuDynamicText MENU_OPTIONS_DYNAMIC_TEXTS[MENU_OPTIONS_NUM_DYNAMIC_TEXTS] = {
	{{.25f, .5f}, {.5f, .25f}, DYNAMIC_TEXT_TYPE_RESOLUTION}};

const Menu MENUS[NUM_MENUS] = {
	[MENU_MAIN] = {
		&on_pressed_quit, NULL,
		MENU_MAIN_NUM_BUTTONS, MENU_MAIN_BUTTONS,
		MENU_MAIN_NUM_DYNAMIC_TEXTS, NULL},
	[MENU_OPTIONS] = {
		&on_pressed_change_menu, &MENU_MAIN_INDEX,
		MENU_OPTIONS_NUM_BUTTONS, MENU_OPTIONS_BUTTONS,
		MENU_OPTIONS_NUM_DYNAMIC_TEXTS, MENU_OPTIONS_DYNAMIC_TEXTS}};

char buffer[31];

void on_pressed_exit(Context *context, MenuContext *menu_context, const void *data, int input)
{
	(void)context;
	(void)data;
	(void)input;
	menu_context->exit = true;
}

void on_pressed_quit(Context *context, MenuContext *menu_context, const void *data, int input)
{
	(void)menu_context;
	(void)data;
	(void)input;
	context->quit = true;
}

void on_pressed_change_menu(Context *context, MenuContext *menu_context, const void *data, int input)
{
	(void)context;
	(void)input;
	menu_context->selected_button = 0;
	menu_context->selected_menu = *((int*) data);
}

void on_pressed_increment(Context *context, MenuContext *menu_context, const void *data, int input)
{
	(void)menu_context;
	ContextIncrementData context_increment_data = *((ContextIncrementData*) data);
	bool *bool_change = (bool*) ((char*) context + context_increment_data.bool_offset);
	int *value = (int*) ((char*) context + context_increment_data.value_offset);
	*bool_change = true;
	switch(input) {
		case I_LEFT:
		if(--(*value) < context_increment_data.min) *value = context_increment_data.max;
		break;
		case I_RIGHT:
		if(++(*value) > context_increment_data.max) *value = context_increment_data.min;
		break;
	}
}

char *get_dynamic_text(Context *context, MenuContext *menu_context, const int text_type)
{
	(void)menu_context;
	switch(text_type)
	{
		case TEXT_RESOLUTION: ;
		const int *resolution = RESOLUTIONS[context->resolution_index];
		sprintf(buffer, "%d x %d", resolution[X], resolution[Y]);
		break;
	}
	return buffer;

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
