#include "checkers-gui.h"

SDL_Texture *load_texture(SDL_Renderer* rend, const char *path, int w, int h)
{
	SDL_Texture *texture = SDL_CreateTextureFromSurface(rend, IMG_Load(path));
	return texture;
}

void draw_board(SDL_Renderer* rend, SDL_Texture* wK, SDL_Texture* wM, SDL_Texture* bK, SDL_Texture* bM, SDL_Texture* board_texture, int *board)
{
	SDL_RenderClear(rend);
	SDL_Rect dest;
	dest.w = BOARD_SIDELENGTH;
	dest.h = BOARD_SIDELENGTH;
	dest.x = 0;
	dest.y = 0;
	SDL_RenderCopy(rend, board_texture, NULL, &dest);
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
				cur_texture = bK;
				break;
			case -1:
				cur_texture = bM;
				break;
			case 1:
				cur_texture = wM;
				break;
			case 2:
				cur_texture = wK;
				break;
			default:
				continue;
		}
		SDL_RenderCopy(rend, cur_texture, NULL, &dest);
	}
	SDL_RenderPresent(rend);
}

int main(int argc, char *argv[])
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
        printf("error initializing SDL: %s\n", SDL_GetError());
    }
    SDL_Window* win = SDL_CreateWindow("DRAUGHTS", // creates a window
                                       SDL_WINDOWPOS_CENTERED,
                                       SDL_WINDOWPOS_CENTERED,
                                       WINDOW_SIZE_X, WINDOW_SIZE_Y, 0);

    Uint32 render_flags = SDL_RENDERER_ACCELERATED;

    SDL_Renderer* rend = SDL_CreateRenderer(win, -1, render_flags);

	SDL_Texture *textures[NUM_TEXTURES];
	int i;
	for(i = 0; i < NUM_TEXTURES; i++)
	    textures[i] = load_texture(rend, texture_filenames[i], texture_sizes[i], texture_sizes[i]);

	int board[50];
	init_board(board);
	draw_board(rend, textures[wK], textures[wM], textures[bK], textures[bM], textures[board_texture], board);

	int close = 0;
    while (!close) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				close = 1;
                break;
			} else if (event.type == SDL_MOUSEBUTTONDOWN) {
				int button = event.button.button;
				if (button == SDL_BUTTON_LEFT) {
                    int x, y;
					SDL_GetMouseState(&x, &y);
					printf("%d : %d\n",x ,y);
                }
			} else {
				continue;
			}
        }
        SDL_Delay(1000 / 60);
    }

	for(i = 0; i < NUM_TEXTURES; i++)
    	SDL_DestroyTexture(textures[i]);

    SDL_DestroyRenderer(rend);

    SDL_DestroyWindow(win);
    return 0;
}
