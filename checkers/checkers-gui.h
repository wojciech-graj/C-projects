#ifndef CHECKERS_GUI_H
#define CHECKERS_GUI_H

#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_thread.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>

#include "checkers.h"

#define BOARD_SIDELENGTH 800
#define WINDOW_SIZE_X 800
#define WINDOW_SIZE_Y 900
#define NUM_TEXTURES 7
#define POLLING_FREQ 60

static const char *PLAYER_COLORS[] = {"White", "Black"};

const int PIECE_SIZE = BOARD_SIDELENGTH / 10;

enum texture_names{wK, wM, bK, bM, board_texture, hightlight_green, hightlight_red};

const char *texture_filenames[] = {"resources/wK.svg",
	"resources/wM.svg",
	"resources/bK.svg",
	"resources/bM.svg",
	"resources/board.png",
	"resources/highlight_green.png",
	"resources/highlight_red.png"};

SDL_Window* win;
SDL_Renderer* rend;
SDL_Texture *textures[NUM_TEXTURES];

Uint32 DRAW_BOARD_EVENT;

int cur_piece = -1;
int cur_destination;
int cur_color = 1;
int cur_board[50];
char players[2] = {'P', 'C'};
int computer_depth[2] = {9, 6};

ListNode *cur_captures = NULL;
Node *cur_capture_node = NULL;

#endif
