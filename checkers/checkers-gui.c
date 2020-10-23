#include "checkers-gui.h"

void end_program()
{
	int i;
	for(i = 0; i < NUM_TEXTURES; i++)
    	SDL_DestroyTexture(textures[i]);

    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);

	exit(0);
}

void end_game(int color)
{
	printf("%s wins!\n", PLAYER_COLORS[(color + 1) / 2]);
	end_program();
}

SDL_Texture *load_texture(const char *path)
{
	SDL_Texture *texture = SDL_CreateTextureFromSurface(rend, IMG_Load(path));
	assert(texture);
	return texture;
}

void draw_board(void)
{
	int *board = cur_board;
	SDL_RenderClear(rend);
	SDL_Rect dest;
	dest.w = BOARD_SIDELENGTH;
	dest.h = BOARD_SIDELENGTH;
	dest.x = 0;
	dest.y = 0;
	SDL_RenderCopy(rend, textures[board_texture], NULL, &dest);
	int i;

	for(i = 0; i < 50; i++)
	{
		dest.w = PIECE_SIZE;
		dest.h = PIECE_SIZE;
		dest.x = (i % 5 * 2 + (int) (i % 10 <= 4)) * PIECE_SIZE;
		dest.y = (i / 5) * PIECE_SIZE;

		if(cur_piece == i) {
			SDL_RenderCopy(rend, textures[hightlight_green], NULL, &dest);
		}

		SDL_Texture *cur_texture;
		switch(board[i])
		{
			case -2:
				cur_texture = textures[bK];
				break;
			case -1:
				cur_texture = textures[bM];
				break;
			case 1:
				cur_texture = textures[wM];
				break;
			case 2:
				cur_texture = textures[wK];
				break;
			default:
				continue;
		}
		SDL_RenderCopy(rend, cur_texture, NULL, &dest);
	}
	SDL_RenderPresent(rend);
}

void play_player_move(int x, int y)
{
	int *board = cur_board;
	int row = y / PIECE_SIZE;
	int board_loc = 5 * row + (x / PIECE_SIZE) / 2;
	if(! SAME_SIGN((2 * (row % 2) - 1), (x % (2 * PIECE_SIZE) - PIECE_SIZE))) {//if selecting dark square
		if(cur_piece != board_loc && SAME_SIGN(board[board_loc], cur_color) && board[board_loc] != 0) {
			cur_piece = board_loc;
		} else if(board[board_loc] == 0 && cur_piece != -1) {
			cur_destination = board_loc;
			int difference = cur_destination - cur_piece + (int) (cur_piece % 10 > 4);
			int direction = 0;
			while(direction <= 3 && NEIGHBORS[direction] != difference) direction++;
			if(direction != 4
				&& ! SAME_SIGN(difference, cur_color)
				&& NOT_OVER_EDGE(cur_piece, cur_destination, direction, 1)) {
				execute_move(board, cur_piece, cur_destination);
				cur_color *= -1;
			} else {
				cur_piece = -1;
			}
		}
	}
}

int play_game(void *ptr)
{
	(void) ptr;

	SDL_Event draw_event;
	SDL_zero(draw_event);
	draw_event.type = DRAW_BOARD_EVENT;

	init_board(cur_board);
	SDL_PushEvent(&draw_event);

    while(true) {
		if(players[(1 - cur_color) / 2] == 'C') {
			(void) play_engine_move(cur_color, cur_board, computer_depth[(1 - cur_color) / 2], true, MIN_EVAL, -MIN_EVAL);
			SDL_PushEvent(&draw_event);
			cur_color *= -1;
		} else if(players[(1 - cur_color) / 2] == 'P') {
			int color = cur_color;
			while(cur_color == color)
			{
				SDL_Delay(1000 / POLLING_FREQ);
			}
		}
    }
}

int main(int argc, char *argv[])
{
	int init = SDL_Init(SDL_INIT_EVERYTHING);
    assert(init == 0);

    win = SDL_CreateWindow("DRAUGHTS",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		WINDOW_SIZE_X, WINDOW_SIZE_Y, 0);
	assert(win);

	Uint32 render_flags = SDL_RENDERER_ACCELERATED;
    rend = SDL_CreateRenderer(win, -1, render_flags);
	assert(rend);

	int i;
	for(i = 0; i < NUM_TEXTURES; i++)
	    textures[i] = load_texture(texture_filenames[i]);

	DRAW_BOARD_EVENT = SDL_RegisterEvents(1);
	assert(DRAW_BOARD_EVENT);

	SDL_Thread *game_thread = SDL_CreateThread(play_game, "game_thread", (void *)NULL);
	assert(game_thread);

	SDL_Event event;
	while(true)
	{
		while (SDL_WaitEvent(&event) >= 0) {
			if (event.type == SDL_QUIT) {
				end_program();
			} else if(event.type == SDL_MOUSEBUTTONDOWN) {
				if(event.button.button == SDL_BUTTON_LEFT && players[(1 - cur_color) / 2] == 'P') {
					int x, y;
					SDL_GetMouseState(&x, &y);
					play_player_move(x, y);
					draw_board();
				}
			} else if(event.type == DRAW_BOARD_EVENT) {
				draw_board();
			}
		}
	}
}
