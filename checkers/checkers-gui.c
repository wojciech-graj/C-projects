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
	w_color = -1 * color;
	playing = false;
}

SDL_Texture *load_texture(const char *path)
{
	SDL_Texture *texture = SDL_CreateTextureFromSurface(rend, IMG_Load(path));
	assert(texture);
	return texture;
}

void draw_texture(int x, int y, int w, int h, int texture_id)
{
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = w;
	rect.h = h;
	SDL_RenderCopy(rend, textures[texture_id], NULL, &rect);
}

void change_color()
{
	prev_piece = cur_piece;
	prev_destination = cur_destination;
	cur_piece = -1;
	cur_destination = -1;
	cur_color *= -1;
}

void draw_board(void)
{
	int *board = cur_board;
	SDL_RenderClear(rend);

	draw_texture(0, 0, BOARD_SIDELENGTH, BOARD_SIDELENGTH, board_texture);

	//Draw winner text
	if(!playing) {
		if(w_color == 1) {
			draw_texture(0, BOARD_SIDELENGTH, 560, PIECE_SIZE, wW);
		} else if(w_color == -1) {
			draw_texture(0, BOARD_SIDELENGTH, 560, PIECE_SIZE, bW);
		}
		draw_texture(560, BOARD_SIDELENGTH, PIECE_SIZE, PIECE_SIZE, replay);
	}

	//draw flip and resign buttons
	draw_texture(BOARD_SIDELENGTH - PIECE_SIZE, BOARD_SIDELENGTH, PIECE_SIZE, PIECE_SIZE, flag);
	draw_texture(BOARD_SIDELENGTH - 2 * PIECE_SIZE, BOARD_SIDELENGTH, PIECE_SIZE, PIECE_SIZE, rotate);

	//draw highlights for captures
	if(cur_capture_node) {
		Node *cur_node = cur_capture_node;
		while(cur_node->parent)
		{
			draw_texture((cur_node->destination % 5 * 2 + (int) (cur_node->destination % 10 <= 4)) * PIECE_SIZE, (cur_node->destination / 5) * PIECE_SIZE, PIECE_SIZE, PIECE_SIZE, highlight_red);
			cur_node = cur_node->parent;
		}
	}

	int i;
	for(i = 0; i < BOARD_SIZE; i++)
	{

		int x = (i % 5 * 2 + (int) (i % 10 <= 4)) * PIECE_SIZE;
		int y = (i / 5) * PIECE_SIZE;
		int piece = flip ? (BOARD_SIZE - i - 1) : i;

		//draw highlights
		if(cur_piece == piece) {
			draw_texture(x, y, PIECE_SIZE, PIECE_SIZE, highlight_green);
		}
		if(prev_piece == piece) {
			draw_texture(x, y, PIECE_SIZE, PIECE_SIZE, highlight_lgreen);
		}
		if(prev_destination == piece) {
			draw_texture(x, y, PIECE_SIZE, PIECE_SIZE, highlight_lred);
		}

		//draw piece
		int cur_texture;
		switch(board[piece])
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
		draw_texture(x, y, PIECE_SIZE, PIECE_SIZE, cur_texture);
	}
	SDL_RenderPresent(rend);
}

void play_player_move(int x, int y)
{
	int *board = cur_board;
	int row = y / PIECE_SIZE;
	int col = x / PIECE_SIZE;
	int board_loc = 5 * (flip ? (9 - row) : row) + (flip ? (9 - col) : col) / 2;
	if(row == 10) {//if selecting menu
		if(col == 8) {//rotate board
			flip = !flip;
		} else if(col == 9) {//resign
			end_game(cur_color);
		}
	} else if(! SAME_SIGN((2 * (row % 2) - 1), (x % (2 * PIECE_SIZE) - PIECE_SIZE))) {//if selecting dark square
		if(cur_piece != board_loc && SAME_SIGN(board[board_loc], cur_color) && board[board_loc] != 0) {
			cur_piece = board_loc;
			cur_capture_node = NULL;
		} else if(cur_piece != -1 && board[board_loc] == 0) {
			cur_destination = board_loc;
			if(cur_captures) {//if can capture
				ListNode *cur_listnode = cur_captures;
				if(! cur_capture_node) {
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
					change_color();
					return;
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
						change_color();
						return;
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
								change_color();
								return;
							}
							temp_piece = next_temp_piece;
							next_temp_piece += NEIGHBOR_DIFF(next_temp_piece, direction);
						}
					}
				}
				cur_piece = -1;
			}
		} else {
			cur_capture_node = NULL;
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

    while(playing) {
		int new_board[BOARD_SIZE];
		memcpy(new_board, cur_board, BOARD_SIZE * sizeof(int));

		if(players[(1 - cur_color) / 2] == 'C') {
			(void) play_engine_move(cur_color, cur_board, computer_depth[(1 - cur_color) / 2], true, MIN_EVAL, -MIN_EVAL);

			//calculate where previous move highlights should go
			int i;
			for(i = 0; i < BOARD_SIZE; i++)
			{
				if(new_board[i] != cur_board[i]) {
					if(SAME_SIGN(new_board[i], cur_color) && new_board[i] != 0) {
						cur_piece = i;
					} else if(SAME_SIGN(cur_board[i], cur_color) && new_board[i] == 0) {
						cur_destination = i;
					}
				}
			}

			change_color();
			SDL_PushEvent(&draw_event);
		} else if(players[(1 - cur_color) / 2] == 'P') {
			//check if current player has moves
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
	return 0;
}

void draw_menu(bool show_begin)
{
	SDL_RenderClear(rend);

	draw_texture(0, 0, PIECE_SIZE * 5, PIECE_SIZE, wMenu);
	draw_texture(0, PIECE_SIZE, PIECE_SIZE * 8, PIECE_SIZE, playerMenu);
	draw_texture(0, PIECE_SIZE * 4, PIECE_SIZE * 5, PIECE_SIZE, bMenu);
	draw_texture(0, PIECE_SIZE * 5, PIECE_SIZE * 8, PIECE_SIZE, playerMenu);

	int i;
	for(i = 0; i < 2; i++)
	{
		if(players[i] == 'C') {
			draw_texture(0, (4 * i + 1) * PIECE_SIZE, 172, PIECE_SIZE, highlight_green);
			draw_texture(0, (4 * i + 2) * PIECE_SIZE, 720, PIECE_SIZE, depthMenu);
			if(computer_depth[i] != 0) {
				draw_texture((computer_depth[i] - 1) * PIECE_SIZE, (4 * i + 2) * PIECE_SIZE, PIECE_SIZE, PIECE_SIZE, highlight_green);
			}
		} else if(players[i] == 'P') {
			draw_texture(231, (4 * i + 1) * PIECE_SIZE, 342, PIECE_SIZE, highlight_green);
		}
	}
	if(show_begin) draw_texture(0, 7 * PIECE_SIZE, 320, PIECE_SIZE, startMenu);
	SDL_RenderPresent(rend);
}

void get_players()
{
	bool show_start = false;
	draw_menu(show_start);
	SDL_Event event;
	while(true)
	{
		while (SDL_WaitEvent(&event) >= 0) {
			if (event.type == SDL_QUIT) {
				end_program();
			} else if(event.type == SDL_MOUSEBUTTONDOWN) {
				if(event.button.button == SDL_BUTTON_LEFT) {
					int x, y;
					SDL_GetMouseState(&x, &y);
					int row = y / PIECE_SIZE;
					int col = x / PIECE_SIZE;
					if(row == 1 || row == 5) {
						if(col >= 0 && col <= 1) {
							players[(row - 1) / 4] = 'C';
						} else if(col >= 3 && col <= 6) {
							players[(row - 1) / 4] = 'P';
							computer_depth[(row - 1) / 4] = 0;
						}
					} else if(row == 2 || row == 6) {
						if(col >= 0 && col <= 8) {
							computer_depth[(row - 1) / 4] = col + 1;
						}
					}
					if((players[0] == 'P' || (players[0] == 'C' && computer_depth[0] != 0))
						&& (players[1] == 'P' || (players[1] == 'C' && computer_depth[1] != 0))) {
						show_start = true;
						if(row == 7 && col >= 0 && col <= 4) return;
					} else {
						show_start = false;
					}
					draw_menu(show_start);
				}
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

	get_players();

	SDL_Thread *game_thread = SDL_CreateThread(play_game, "game_thread", (void *)NULL);
	assert(game_thread);

	SDL_Event event;
	while(true)
	{
		while (SDL_WaitEvent(&event) >= 0) {
			if (event.type == SDL_QUIT) {
				end_program();
			} else if(event.type == SDL_MOUSEBUTTONDOWN) {
				if(event.button.button == SDL_BUTTON_LEFT) {
					int x, y;
					SDL_GetMouseState(&x, &y);
					if(players[(1 - cur_color) / 2] == 'P' && playing) {
						play_player_move(x, y);
					} else if(! playing && y / PIECE_SIZE == 10 && x / PIECE_SIZE == 7) {//restart
						playing = true;
						cur_piece = -1;
						cur_destination = -1;
						prev_piece = -1;
						prev_destination = -1;
						if(cur_captures) {
							delete_list(cur_captures);
							cur_captures = NULL;
						}
						cur_capture_node = NULL;
						get_players();
						cur_color = 1;
						SDL_Thread *game_thread = SDL_CreateThread(play_game, "game_thread", (void *)NULL);
						assert(game_thread);
					}
					draw_board();
				}
			} else if(event.type == DRAW_BOARD_EVENT) {
				draw_board();
			}
		}
	}
}
