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

SDL_Texture *load_texture(SDL_Renderer* rend, const char *path, int w, int h)
{
	SDL_Texture *texture = SDL_CreateTextureFromSurface(rend, IMG_Load(path));
	assert(texture);
	return texture;
}

void draw_board(int *board)
{
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

void play_player_move(int color, int *board)
{
	int piece = -1;
	int destination;
	SDL_Event event;
	while(true)
	{
		while (SDL_PollEvent(&event)) {
			 if (event.type == SDL_MOUSEBUTTONDOWN) {
				if (event.button.button == SDL_BUTTON_LEFT) {
					int x, y;
					SDL_GetMouseState(&x, &y);
					int row = y / PIECE_SIZE;
					int board_loc = 5 * row + (x / PIECE_SIZE) / 2;
					if(! SAME_SIGN((2 * (row % 2) - 1), (x % (2 * PIECE_SIZE) - PIECE_SIZE))) {//if selecting dark square
						if(SAME_SIGN(board[board_loc], color) && board[board_loc] != 0) {
							piece = board_loc;
						} else if(board[board_loc] == 0) {
							destination = board_loc;
							//check if can move
						}
					}
				}
				//break;
			}
		}
		SDL_Delay(1000 / POLLING_FREQ);
	}
}

int check_if_end_program(void *ptr)
{
	(void) ptr;
	SDL_Event event;
	while(true)
	{
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				end_program();
			}
		}
		SDL_Delay(1000 / POLLING_FREQ);
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
	    textures[i] = load_texture(rend, texture_filenames[i], texture_sizes[i], texture_sizes[i]);

	int board[BOARD_SIZE];
	init_board(board);

	draw_board(board);

	SDL_Thread *thread = SDL_CreateThread(check_if_end_program, "check_if_end_program_thread", (void *)NULL);
	assert(thread);

	//TODO: Allow user to select who plays
	char players[2] = {'C', 'C'};
	int computer_depth[2] = {9, 6};

	i=0;
	int color = 1;
    while(true) {
		if(players[i % 2] == 'C') {
			(void) play_engine_move(color, board, computer_depth[i % 2], true, MIN_EVAL, -MIN_EVAL);
		} else if(players[i % 2] == 'P') {
			play_player_move(color, board);
		}
		i++;
		color *= -1;
		draw_board(board);
    }
    return 0;
}
