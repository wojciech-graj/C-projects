#ifndef MENU_P_H
#define MENU_P_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "inc/context.h"
#include "inc/config.h"

#define MENU_MAIN_NUM_BUTTONS 3
#define MENU_OPTIONS_NUM_BUTTONS 3
#define MENU_KEYBINDS_NUM_BUTTONS 4
#define MENU_MAIN_NUM_DYNAMIC_TEXTS 0
#define MENU_OPTIONS_NUM_DYNAMIC_TEXTS 2
#define MENU_KEYBINDS_NUM_DYNAMIC_TEXTS 4

typedef struct BoolChangeData BoolChangeData;
typedef struct IncrementData IncrementData;

typedef struct BoolChangeData {
	const int context_type;
	const bool value;
	const int offset;
} BoolChangeData;

typedef struct IncrementData {
	const int offset;
	const int min;
	const int max;
	const BoolChangeData *bool_change_data;
} IncrementData;

enum context_types{CONTEXT, CONTEXT_MENU, CONTEXT_CONFIG};
enum dynamic_text_types{TEXT_RESOLUTION, TEXT_KEYBIND, TEXT_VOLUME};

void on_pressed_exit_options(Context *context, MenuContext *menu_context, const void *data, int input);
void on_pressed_rebind_key(Context *context, MenuContext *menu_context, const void *data, int input);
void on_pressed_change_bool(Context *context, MenuContext *menu_context, const void *data, int input);
void on_pressed_change_menu(Context *context, MenuContext *menu_context, const void *data, int input);
void on_pressed_increment_config(Context *context, MenuContext *menu_context, const void *data, int input);

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
const BoolChangeData MENU_BUTTON_VOLUME_SUBDATA = {CONTEXT, true, offsetof(Context, volume_change)};

const IncrementData MENU_BUTTON_RESOLUTION_DATA = {offsetof(Config, resolution_index), 0, NUM_RESOLUTIONS - 1, &MENU_BUTTON_RESOLUTION_SUBDATA};
const IncrementData MENU_BUTTON_VOLUME_DATA = {offsetof(Config, volume), 0, 10, &MENU_BUTTON_VOLUME_SUBDATA};

const MenuButton MENU_MAIN_BUTTONS[MENU_MAIN_NUM_BUTTONS] = {
	{{.1f, .25f}, {.5f, .15f}, "PLAY", &on_pressed_change_bool, &MENU_BUTTON_PLAY_DATA},
	{{.1f, .4f}, {.5f, .15f}, "OPTIONS", &on_pressed_change_menu, &MENU_OPTIONS_INDEX},
	{{.1f, .55f}, {.5f, .15f}, "QUIT", &on_pressed_change_bool, &MENU_BUTTON_QUIT_DATA}};
const MenuButton MENU_OPTIONS_BUTTONS[MENU_OPTIONS_NUM_BUTTONS] = {
	{{.1f, .25f}, {.4f, .1f}, "RESOLUTION", &on_pressed_increment_config, &MENU_BUTTON_RESOLUTION_DATA},
	{{.1f, .35f}, {.4f, .1f}, "VOLUME", &on_pressed_increment_config, &MENU_BUTTON_VOLUME_DATA},
	{{.1f, .45f}, {.4f, .1f}, "KEYBINDS", &on_pressed_change_menu, &MENU_KEYBINDS_INDEX}};
const MenuButton MENU_KEYBINDS_BUTTONS[MENU_KEYBINDS_NUM_BUTTONS] = {
	{{.1f, .25f}, {.4f, .1f}, "LEFT", &on_pressed_rebind_key, &MENU_INPUT_KEY_INDEXES[I_LEFT]},
	{{.1f, .35f}, {.4f, .1f}, "RIGHT", &on_pressed_rebind_key, &MENU_INPUT_KEY_INDEXES[I_RIGHT]},
	{{.1f, .45f}, {.4f, .1f}, "UP", &on_pressed_rebind_key, &MENU_INPUT_KEY_INDEXES[I_UP]},
	{{.1f, .55f}, {.4f, .1f}, "DOWN", &on_pressed_rebind_key, &MENU_INPUT_KEY_INDEXES[I_DOWN]},};

const MenuDynamicText MENU_OPTIONS_DYNAMIC_TEXTS[MENU_OPTIONS_NUM_DYNAMIC_TEXTS] = {
	{{.5f, .25f}, {.4f, .1f}, TEXT_RESOLUTION, NULL},
	{{.5f, .35f}, {.4f, .1f}, TEXT_VOLUME, NULL}};
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

#endif
