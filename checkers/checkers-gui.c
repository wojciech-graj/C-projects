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

	dest.w = PIECE_SIZE;
	dest.h = PIECE_SIZE;

	//draw highlights for captures
	if(cur_capture_node) {
		Node *cur_node = cur_capture_node;
		while(cur_node->parent)
		{
			dest.x = (cur_node->destination % 5 * 2 + (int) (cur_node->destination % 10 <= 4)) * PIECE_SIZE;
			dest.y = (cur_node->destination / 5) * PIECE_SIZE;
			SDL_RenderCopy(rend, textures[hightlight_red], NULL, &dest);
			cur_node = cur_node->parent;
		}
	}

	//draw pieces
	int i;
	for(i = 0; i < 50; i++)
	{
		dest.x = (i % 5 * 2 + (int) (i % 10 <= 4)) * PIECE_SIZE;
		dest.y = (i / 5) * PIECE_SIZE;

		//draw highlight for selected piece
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
		} else if(cur_piece != -1 && board[board_loc] == 0) {
			cur_destination = board_loc;
			if(cur_captures) {//if can capture
				ListNode *cur_listnode = cur_captures;
				if(! cur_capture_node) { //TODO: OPTIMIZE
					while(cur_listnode) {
						Node *cur_node = cur_listnode->node;
						while(cur_node->parent->parent) {
							cur_node = cur_node->parent;
						}
						if(cur_node->piece == cur_piece && cur_node->destination == cur_destination) {
							cur_capture_node = cur_node;
							goto END_OF_CAPTURES;
						}
						cur_listnode = cur_listnode->node_next;
					}
				} else {
					while(cur_listnode) {
						Node *cur_node = cur_listnode->node;
						while(cur_node->parent->parent) {
							if(cur_node->parent == cur_capture_node && cur_destination == cur_node->destination) {
								cur_capture_node = cur_node;
								goto END_OF_CAPTURES;
							}
							cur_node = cur_node->parent;
						}
						cur_listnode = cur_listnode->node_next;
					}
					cur_capture_node = NULL;
				}
				cur_piece = -1;
				return;
				END_OF_CAPTURES:
				if(! cur_capture_node->child) {
					execute_captures(cur_board, cur_capture_node);
					cur_capture_node = NULL;
					cur_color *= -1;
				}
			} else if(! cur_captures) {
				int difference = cur_destination - cur_piece;
				if(fabs(board[cur_piece]) == 1) {
					difference += (int) (cur_piece % 10 > 4);
					int direction = 0;
					while(direction <= 3 && NEIGHBORS[direction] != difference) direction++;
					if(direction != 4
						&& ! SAME_SIGN(difference, cur_color)
						&& NOT_OVER_EDGE(cur_piece, cur_destination, direction, 1)) {
						execute_move(board, cur_piece, cur_destination);
						cur_color *= -1;
					}
				} else { //if queen
					int direction;
					for(direction = 2 * (int) (difference > 0); direction < 2 + 2 * (int) (difference > 0); direction++) //only check directions with same sign as difference, either 0,1 or 2,3
					{
						int temp_piece = cur_piece;
						int next_temp_piece = cur_piece + NEIGHBOR_DIFF(cur_piece, direction);
						while(NOT_OVER_EDGE(temp_piece, next_temp_piece, direction, 1) && cur_board[next_temp_piece] == 0)
						{
							if(next_temp_piece == cur_destination) {
								execute_move(cur_board, cur_piece, cur_destination);
								cur_color *= -1;
								return;
							}
							temp_piece = next_temp_piece;
							next_temp_piece += NEIGHBOR_DIFF(next_temp_piece, direction);
						}
					}
				}
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

	cur_board[27] = -1;
	cur_board[10] = 0;
	cur_board[1] = 0;

	SDL_PushEvent(&draw_event);

    while(true) {
		if(players[(1 - cur_color) / 2] == 'C') {
			(void) play_engine_move(cur_color, cur_board, computer_depth[(1 - cur_color) / 2], true, MIN_EVAL, -MIN_EVAL);
			SDL_PushEvent(&draw_event);
			cur_color *= -1;
		} else if(players[(1 - cur_color) / 2] == 'P') {
			//check if current player has moves
			int new_board[BOARD_SIZE];
			memcpy(new_board, cur_board, BOARD_SIZE * sizeof(int));
			(void) play_engine_move(cur_color, new_board, 0, true, -MIN_EVAL, MIN_EVAL);

			Node *head;
			init_node(&head);
			int max_depth = create_capture_tree(cur_color, cur_board, head);
			if(head->child) get_nodes_at_depth(head, 0, max_depth, &cur_captures);

			int color = cur_color;
			while(cur_color == color)
			{
				SDL_Delay(1000 / POLLING_FREQ);
			}

			//clean up
			delete_tree(head);
			if(cur_captures) {
				delete_list(cur_captures);
				cur_captures = NULL;
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
