#ifndef MENU_H
#define MENU_H

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "global.h"
#include "context.h"
#include "config.h"

#define NUM_MENUS 3
#define MENU_MAIN_NUM_BUTTONS 3
#define MENU_OPTIONS_NUM_BUTTONS 2
#define MENU_KEYBINDS_NUM_BUTTONS 4
#define MENU_MAIN_NUM_DYNAMIC_TEXTS 0
#define MENU_OPTIONS_NUM_DYNAMIC_TEXTS 1
#define MENU_KEYBINDS_NUM_DYNAMIC_TEXTS 4

enum menu_presets{MENU_MAIN, MENU_OPTIONS, MENU_KEYBINDS};
enum context_types{CONTEXT, CONTEXT_MENU, CONTEXT_CONFIG};
enum dynamic_text_types{TEXT_RESOLUTION, TEXT_KEYBIND};

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

typedef struct Menu {
	void (*on_escape)(Context*, MenuContext*, const void*, int);
	const void *data;
	const int num_buttons;
	const MenuButton *buttons;
	const int num_dynamic_texts;
	const MenuDynamicText *dynamic_texts;
} Menu;

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

MenuContext *init_menu_context(int menu_index);
void delete_menu_context(MenuContext *menu_context);

char *get_dynamic_text(Context *context, MenuContext *menu_context, const MenuDynamicText dynamic_text);

void on_pressed_exit_options(Context *context, MenuContext *menu_context, const void *data, int input);
void on_pressed_rebind_key(Context *context, MenuContext *menu_context, const void *data, int input);
void on_pressed_change_bool(Context *context, MenuContext *menu_context, const void *data, int input);
void on_pressed_change_menu(Context *context, MenuContext *menu_context, const void *data, int input);
void on_pressed_increment_config(Context *context, MenuContext *menu_context, const void *data, int input);

extern const Menu MENUS[NUM_MENUS];

#endif
