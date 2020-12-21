#include "config.h"

Config *init_config(void)
{
	Config *config = malloc(sizeof(Config));
	*config = (Config) {
		.resolution_index = DEFAULT_RESOLUTION_INDEX,
		.input_keycodes = {
		[I_LEFT] = SDLK_LEFT,
		[I_UP] = SDLK_UP,
		[I_RIGHT] = SDLK_RIGHT,
		[I_DOWN] = SDLK_DOWN,
		[I_RETURN] = SDLK_RETURN,
		[I_ESCAPE] = SDLK_ESCAPE},
		.input_scancodes = {
		[I_LEFT] = SDL_SCANCODE_LEFT,
		[I_UP] = SDL_SCANCODE_UP,
		[I_RIGHT] = SDL_SCANCODE_RIGHT,
		[I_DOWN] = SDL_SCANCODE_DOWN,
		[I_RETURN] = SDL_SCANCODE_RETURN,
		[I_ESCAPE] = SDL_SCANCODE_ESCAPE}};
	return config;
}

Config *load_config(const char *filename)
{
	FILE *file = fopen(filename, "rb");
	if(file) {
		Config *config = malloc(sizeof(Config));
		assert(fread(config, sizeof(Config), 1, file) == 1);
		fclose(file);
		return config;
	} else {
		return init_config();
	}
}

void save_config(const char *filename, Config *config)
{
	FILE *file = fopen(filename, "w+b");
	fwrite(config, sizeof(Config), 1, file);
	fclose(file);
}
