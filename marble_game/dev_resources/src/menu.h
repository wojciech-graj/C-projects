#ifndef MENU_H
#define MENU_H

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "global.h"
#include "context.h"

#define NUM_MENUS 2
#define MENU_MAIN_NUM_BUTTONS 3
#define MENU_OPTIONS_NUM_BUTTONS 1
#define MENU_MAIN_NUM_DYNAMIC_TEXTS 0
#define MENU_OPTIONS_NUM_DYNAMIC_TEXTS 1

enum menu_presets{MENU_MAIN, MENU_OPTIONS};

typedef struct MenuContext MenuContext;
typedef struct Menu Menu;
typedef struct MenuButton MenuButton;
typedef struct MenuDynamicText MenuDynamicText;

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
	void (*on_pressed)(Context*, MenuContext*, const void*, int);
	const void *data;
} MenuButton;

typedef struct MenuDynamicText {
	const float position[2];
	const float size[2];
	const int text_type;
} MenuDynamicText;

typedef struct Menu {
	void (*on_escape)(Context*, MenuContext*, const void*, int);
	const void *data;
	const int num_buttons;
	const MenuButton *buttons;
	const int num_dynamic_texts;
	const MenuDynamicText *dynamic_texts;
} Menu;

typedef struct ContextIncrementData {
	const int value_offset;
	const int bool_offset;
	const int min;
	const int max;
} ContextIncrementData;

enum dynamic_text_types{TEXT_RESOLUTION};

MenuContext *init_menu_context(int menu_index);
void delete_menu_context(MenuContext *menu_context);

char *get_dynamic_text(Context *context, MenuContext *menu_context, const int text_type);

void on_pressed_exit(Context *context, MenuContext *menu_context, const void *data, int input);
void on_pressed_quit(Context *context, MenuContext *menu_context, const void *data, int input);
void on_pressed_change_menu(Context *context, MenuContext *menu_context, const void *data, int input);
void on_pressed_increment(Context *context, MenuContext *menu_context, const void *data, int input);

extern const Menu MENUS[NUM_MENUS];

#endif
