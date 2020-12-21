#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <assert.h>

#include <SDL2/SDL.h>

#include "global.h"

typedef struct Config {
	int resolution_index;
	SDL_Keycode input_keycodes[NUM_INPUTS];
	SDL_Scancode input_scancodes[NUM_INPUTS];
} Config;

Config *init_config(void);
Config *load_config(const char *filename);
void save_config(const char *filename, Config *config);

#endif
