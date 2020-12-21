#include "menu.h"

const int MENU_MAIN_INDEX = MENU_MAIN;
const int MENU_OPTIONS_INDEX = MENU_OPTIONS;
const int MENU_KEYBINDS_INDEX = MENU_KEYBINDS;

const int MENU_INPUT_KEY_INDEXES[NUM_INPUTS] = {
	[I_LEFT] = I_LEFT,
	[I_UP] = I_UP,
	[I_RIGHT] = I_RIGHT,
	[I_DOWN] = I_DOWN,
	[I_RETURN] = I_RETURN,
	[I_ESCAPE] = I_ESCAPE};

const BoolChangeData MENU_BUTTON_PLAY_DATA = {CONTEXT_MENU, true, offsetof(MenuContext, exit)};
const BoolChangeData MENU_BUTTON_QUIT_DATA = {CONTEXT, true, offsetof(Context, quit)};
const BoolChangeData MENU_BUTTON_RESOLUTION_SUBDATA = {CONTEXT, true, offsetof(Context, resize)};

const IncrementData MENU_BUTTON_RESOLUTION_DATA = {offsetof(Config, resolution_index), 0, NUM_RESOLUTIONS - 1, &MENU_BUTTON_RESOLUTION_SUBDATA};

const MenuButton MENU_MAIN_BUTTONS[MENU_MAIN_NUM_BUTTONS] = {
	{{.25f, .25f}, {.5f, .15f}, "PLAY", &on_pressed_change_bool, &MENU_BUTTON_PLAY_DATA},
	{{.25f, .4f}, {.5f, .15f}, "OPTIONS", &on_pressed_change_menu, &MENU_OPTIONS_INDEX},
	{{.25f, .55f}, {.5f, .15f}, "QUIT", &on_pressed_change_bool, &MENU_BUTTON_QUIT_DATA}};
const MenuButton MENU_OPTIONS_BUTTONS[MENU_OPTIONS_NUM_BUTTONS] = {
	{{.25f, .25f}, {.5f, .1f}, "RESOLUTION", &on_pressed_increment_config, &MENU_BUTTON_RESOLUTION_DATA},
	{{.25f, .35f}, {.5f, .1f}, "KEYBINDS", &on_pressed_change_menu, &MENU_KEYBINDS_INDEX}};
const MenuButton MENU_KEYBINDS_BUTTONS[MENU_KEYBINDS_NUM_BUTTONS] = {
	{{.25f, .25f}, {.5f, .1f}, "LEFT", &on_pressed_rebind_key, &MENU_INPUT_KEY_INDEXES[I_LEFT]},
	{{.25f, .35f}, {.5f, .1f}, "RIGHT", &on_pressed_rebind_key, &MENU_INPUT_KEY_INDEXES[I_RIGHT]},
	{{.25f, .45f}, {.5f, .1f}, "UP", &on_pressed_rebind_key, &MENU_INPUT_KEY_INDEXES[I_UP]},
	{{.25f, .55f}, {.5f, .1f}, "DOWN", &on_pressed_rebind_key, &MENU_INPUT_KEY_INDEXES[I_DOWN]},};

const MenuDynamicText MENU_OPTIONS_DYNAMIC_TEXTS[MENU_OPTIONS_NUM_DYNAMIC_TEXTS] = {
	{{.25f, .5f}, {.5f, .1f}, TEXT_RESOLUTION, NULL}};
const MenuDynamicText MENU_KEYBINDS_DYNAMIC_TEXTS[MENU_KEYBINDS_NUM_DYNAMIC_TEXTS] = {
	{{.5f, .25f}, {.5f, .1f}, TEXT_KEYBIND, &MENU_INPUT_KEY_INDEXES[I_LEFT]},
	{{.5f, .35f}, {.5f, .1f}, TEXT_KEYBIND, &MENU_INPUT_KEY_INDEXES[I_RIGHT]},
	{{.5f, .45f}, {.5f, .1f}, TEXT_KEYBIND, &MENU_INPUT_KEY_INDEXES[I_UP]},
	{{.5f, .55f}, {.5f, .1f}, TEXT_KEYBIND, &MENU_INPUT_KEY_INDEXES[I_DOWN]}};

const Menu MENUS[NUM_MENUS] = {
	[MENU_MAIN] = {
		&on_pressed_change_bool, &MENU_BUTTON_QUIT_DATA,
		MENU_MAIN_NUM_BUTTONS, MENU_MAIN_BUTTONS,
		MENU_MAIN_NUM_DYNAMIC_TEXTS, NULL},
	[MENU_OPTIONS] = {
		&on_pressed_exit_options, NULL,
		MENU_OPTIONS_NUM_BUTTONS, MENU_OPTIONS_BUTTONS,
		MENU_OPTIONS_NUM_DYNAMIC_TEXTS, MENU_OPTIONS_DYNAMIC_TEXTS},
	[MENU_KEYBINDS] = {
		&on_pressed_change_menu, &MENU_OPTIONS_INDEX,
		MENU_KEYBINDS_NUM_BUTTONS, MENU_KEYBINDS_BUTTONS,
		MENU_KEYBINDS_NUM_DYNAMIC_TEXTS, MENU_KEYBINDS_DYNAMIC_TEXTS}};

char buffer[31];

void on_pressed_rebind_key(Context *context, MenuContext *menu_context, const void *data, int input)
{
	(void) input;
	context->gamestate = STATE_KEYBIND;
	menu_context->rebound_input = *((int*) data);
}

void on_pressed_change_bool(Context *context, MenuContext *menu_context, const void *data, int input)
{
	(void)input;
	BoolChangeData bool_change_data = *((BoolChangeData*) data);
	switch(bool_change_data.context_type)
	{
		case CONTEXT:
		*((bool*)((char*) context + bool_change_data.offset)) = bool_change_data.value;
		break;
		case CONTEXT_MENU:
		*((bool*)((char*) menu_context + bool_change_data.offset)) = bool_change_data.value;
		break;
	}
}

void on_pressed_exit_options(Context *context, MenuContext *menu_context, const void *data, int input)
{
	(void)data;
	(void)input;
	save_config("config", context->config);
	menu_context->selected_button = 0;
	menu_context->selected_menu = MENU_MAIN;
}

void on_pressed_change_menu(Context *context, MenuContext *menu_context, const void *data, int input)
{
	(void)context;
	(void)input;
	menu_context->selected_button = 0;
	menu_context->selected_menu = *((int*) data);
}

void on_pressed_increment_config(Context *context, MenuContext *menu_context, const void *data, int input)
{
	IncrementData increment_data = *((IncrementData*) data);
	int *value = (int*) ((char*) context->config + increment_data.offset);
	switch(input) {
		case I_LEFT:
		if(--(*value) < increment_data.min) *value = increment_data.max;
		break;
		case I_RIGHT:
		if(++(*value) > increment_data.max) *value = increment_data.min;
		break;
	}
	if(increment_data.bool_change_data) {
		on_pressed_change_bool(context, menu_context, increment_data.bool_change_data, input);
	}
}

char *get_dynamic_text(Context *context, MenuContext *menu_context, const MenuDynamicText dynamic_text)
{
	(void)menu_context;
	switch(dynamic_text.text_type)
	{
		case TEXT_RESOLUTION: ;
		const int *resolution = RESOLUTIONS[context->config->resolution_index];
		sprintf(buffer, "%d x %d", resolution[X], resolution[Y]);
		return buffer;
		break;
		case TEXT_KEYBIND: ;
		return (char*) SDL_GetKeyName(context->config->input_keycodes[*((int*) dynamic_text.data)]);
		break;
	}
	return "";
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

void delete_menu_context(MenuContext *menu_context)
{
	free(menu_context);
}
