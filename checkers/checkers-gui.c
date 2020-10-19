#include "checkers-gui.h"

void draw_board_edge(WINDOW *window, int x, int y)
{
	int i;
	for(i = 2; i <= BOARD_SIDELENGTH; i += 2)
	{
		mvwprintw(window, y + 1 + i, x, "%d", 5 * i - 4);
		mvwprintw(window, y, x + 2 + i, "%d", i / 2);
	}

	x += 2;
	y += 1;
	for(i = 1; i <= BOARD_SIDELENGTH; i++)
	{
		mvwaddwstr(window, y + i, x, L"║");
		mvwaddwstr(window, y + i, x + 1 + BOARD_SIDELENGTH, L"║");
		mvwaddwstr(window, y, x + i, L"═");
		mvwaddwstr(window, y + 1 + BOARD_SIDELENGTH, x + i, L"═");
	}
	mvwaddwstr(window, y, x, L"╔");
	mvwaddwstr(window, y, x + 1 + BOARD_SIDELENGTH, L"╗");
	mvwaddwstr(window, y + 1 + BOARD_SIDELENGTH, x, L"╚");
	mvwaddwstr(window, y + 1 + BOARD_SIDELENGTH, x + 1 + BOARD_SIDELENGTH, L"╝");
}

void draw_board(WINDOW *window, int *board)
{
	wclear(window);
	int i;
	for(i = 0; i < 2 * BOARD_SIZE; i++)
	{
		wchar_t *tile;
		if(i % 2 == 0 && i % 20 > 9 || i % 2 == 1 && i % 20 < 10) {
			switch(board[i/2])
			{
				case -2:
					tile = L"⛁";
					break;
				case -1:
					tile = L"⛀";
					break;
				case 0:
					tile = L" ";
					break;
				case 1:
					tile = L"⛂";
					break;
				case 2:
					tile = L"⛃";
					break;
			}
		} else {
			tile = L"█";
		}
		waddwstr(window, tile);
	}
}

void end_game(int color)
{
	printf("%s wins!\n", PLAYER_COLORS[(color + 1) / 2]);
	endwin();
	exit(0);
}

void play_player_move(WINDOW *window, WINDOW *board_window, int *board, int color)
{
	MEVENT event;
	while(true)
	{
		int c = wgetch(window);
		if(c == KEY_MOUSE) {
			if(getmouse(&event) == OK) {
				if(event.bstate & BUTTON1_PRESSED) {
					int x = event.x;
					int y = event.y;
					if(x < BOARD_OFFSET[0] + BOARD_SIZE && x >= BOARD_OFFSET[0]
						&& y < BOARD_OFFSET[1] + BOARD_SIZE && y >= BOARD_OFFSET[1]) {
						draw_board(board_window, board);
						wmove(board_window, y - BOARD_OFFSET[1], x - BOARD_OFFSET[0]);
						wrefresh(board_window);
					} else if(x == EXIT_POS[0] && y == EXIT_POS[1]) {
						endwin();
						exit(0);
					} else if(x == RESIGN_POS[0] && y == RESIGN_POS[1]) {
						end_game(color);
					} else if(x == CAPTURE_POS[0] && y == CAPTURE_POS[1]) {

					} else {
						//reset mouse loc
					}
				}
			}
		}
	}
}

int main()
{
	int board[50];
	int i;
	char players[2];
	int computer_depth[2];

	init_board(board);

	setlocale(LC_ALL, "");
	initscr();
	clear();
	noecho();
	cbreak();

	WINDOW *window = newwin(SCREEN_HEIGHT, SCREEN_WIDTH, 0, 0);
	WINDOW *board_window = newwin(BOARD_SIDELENGTH, BOARD_SIDELENGTH, BOARD_OFFSET[1], BOARD_OFFSET[0]);

	wprintw(window, "DRAUGHTS");
	mvwaddch(window, EXIT_POS[1], EXIT_POS[0], 'X');
	mvwprintw(window, 14, 0, "MOVE:");
	mvwaddwstr(window, RESIGN_POS[1], RESIGN_POS[0], L"🏳️");
	mvwaddwstr(window, CAPTURE_POS[1], CAPTURE_POS[0], L"▶️");
	draw_board_edge(window, 0, 1);
	wrefresh(window);

	draw_board(board_window, board);
	wrefresh(board_window);

	mousemask(ALL_MOUSE_EVENTS, NULL);
	keypad(window, TRUE);

	//CHANGE LATER
	players[0] = 'P';
	players[1] = 'P';
	computer_depth[0] = 6;
	computer_depth[1] = 8;

	int color = 1;
	i = 0;
	while(true)
	{
		if(players[i % 2] == 'C') {
			(void) play_engine_move(color, board, computer_depth[i % 2], true, MIN_EVAL, -MIN_EVAL);
		} else if(players[i % 2] == 'P') {
			play_player_move(window, board_window, board, color);
		}
		draw_board(board_window, board);
		wrefresh(board_window);
		i++;
		color *= -1;

	}

	endwin();
	return 0;
}
