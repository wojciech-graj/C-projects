#ifndef CONTEXT_H
#define CONTEXT_H

#include <stdbool.h>

#include "global.h"

typedef struct Context Context;
typedef struct Config Config;
typedef struct Level Level;
typedef struct Sound Sound;

typedef struct Context {
	bool resize;
	bool volume_change;

	int gamestate;
	int timer;

	bool quit;

	float input[2];
	bool can_control;

	short num_textures;
	unsigned int *textures;

	Config *config;

	Level *level;

	Sound *sound;
} Context;

Context *init_context(Config *config);
void delete_context(Context *context);

#endif
