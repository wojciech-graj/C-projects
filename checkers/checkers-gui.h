#ifndef CHECKERS_GUI_H
#define CHECKERS_GUI_H

#include <stdbool.h>
#include <assert.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>

#include "checkers.h"

#define BOARD_SIDELENGTH 800
#define WINDOW_SIZE_X 800
#define WINDOW_SIZE_Y 900
#define NUM_TEXTURES 5
#define FPS 30

const int PIECE_SIZE = BOARD_SIDELENGTH / 10;

enum texture_names{wK, wM, bK, bM, board_texture};
const char *texture_filenames[] = {"resources/wK.svg",
	"resources/wM.svg",
	"resources/bK.svg",
	"resources/bM.svg",
	"resources/board.png"};

const int texture_sizes[] = {PIECE_SIZE,
	PIECE_SIZE,
	PIECE_SIZE,
	PIECE_SIZE,
	BOARD_SIDELENGTH};

SDL_Window* win;
SDL_Renderer* rend;
SDL_Texture *textures[NUM_TEXTURES];

#endif
