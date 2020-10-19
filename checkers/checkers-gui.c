#include <ncurses.h>
#include <locale.h>
#include "checkers.h"

#define BOARD_SIDELENGTH 10

void draw_board_edge(int x, int y)
{
	int i;
	for(i = 2; i <= BOARD_SIDELENGTH; i += 2)
	{
		mvprintw(y + 1 + i, x, "%d", 5 * i - 4);
		mvprintw(y, x + 2 + i, "%d", i / 2);
	}

	x += 2;
	y += 1;
	for(i = 1; i <= BOARD_SIDELENGTH; i++)
	{
		mvaddwstr(y + i, x, L"║");
		mvaddwstr(y + i, x + 1 + BOARD_SIDELENGTH, L"║");
		mvaddwstr(y, x + i, L"═");
		mvaddwstr(y + 1 + BOARD_SIDELENGTH, x + i, L"═");
	}
	mvaddwstr(y, x, L"╔");
	mvaddwstr(y, x + 1 + BOARD_SIDELENGTH, L"╗");
	mvaddwstr(y + 1 + BOARD_SIDELENGTH, x, L"╚");
	mvaddwstr(y + 1 + BOARD_SIDELENGTH, x + 1 + BOARD_SIDELENGTH, L"╝");
	refresh();
}

void draw_board(WINDOW *window, int *board)
{
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
	wrefresh(window);
}

int main()
{
	setlocale(LC_ALL, "");
	initscr();
	clear();
	noecho();
	cbreak();

	printw("TEXT");
	refresh();

	WINDOW *board_window = newwin(BOARD_SIDELENGTH, BOARD_SIDELENGTH, 3, 3);

	int board[50];
	int i;

	for(i=0; i<=19; i++) board[i] = -1;
	for(i=20; i<=29; i++) board[i] = 0;
	for(i=30; i<=49; i++) board[i] = 1;

	draw_board_edge(0, 1);

	draw_board(board_window, board);

	sleep(5);

	endwin();
}
