#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#define BOARD_SIZE  50

const int BUFFER_SIZE = 255;
const int NEIGHBORS[] = {-5, -4, 5, 6};

void print_board(int *board)
{
	printf("   1 2 3 4 5");
	int i;
	for(i = 0; i < BOARD_SIZE * 2; i++)
	{
		char *tile;
		if(i % 10 == 0){
			if(i % 20 == 10) {
				printf("\n%-2d", i / 2 + 1);
			} else {
				printf("\n  ");
			}
		}
		if(i % 2 == 0 && i % 20 > 9 || i % 2 == 1 && i % 20 < 10) {
			switch(board[i/2])
			{
				case -2:
					tile = "⛁";
					break;
				case -1:
					tile = "⛀";
					break;
				case 0:
					tile = " ";
					break;
				case 1:
					tile = "⛂";
					break;
				case 2:
					tile = "⛃";
					break;
			}
		} else {
			tile = "▓";
		}
		printf("%s", tile);
	}
	printf("\n");
}

void get_moves(int color, int *board)
{
	//find forced moves
	int forced_moves[BUFFER_SIZE];
	int list_index = 0;
	int piece;
	for(piece = 0; piece < BOARD_SIZE; piece++)
	{
		if((board[piece] ^ color) >= 0 && board[piece] != 0) {//if piece has same sign
			int direction;
			for(direction = 0; direction < 4; direction++)
			{
				int neighbor = piece + NEIGHBORS[direction] - (int) (piece % 10 > 4); //subtract 1 every other row.
				if(! ((piece < 9 && neighbor < piece)
					|| (piece > 40 && neighbor > piece)
					|| (piece % 5 == 0 && direction % 2 == 0)
					|| (piece % 5 == 4 && direction % 2 == 1))) { //if not capturing over edge of board
					int behind_neighbor = neighbor + NEIGHBORS[direction] - (neighbor % 10 > 4);
					if ((board[neighbor] ^ color) < 0 && board[neighbor] != 0
						&& board[behind_neighbor] == 0) { //if can capture neighbor
							forced_moves[list_index] = piece;
							forced_moves[list_index + 1] = neighbor;
							list_index += 2;
					}
				}
			}
		}
	}
}
/*
void player_move(int player_color)
{
	print_board();
	char buf[BUFFER_SIZE];
	printf("Move? ");
	scanf("%s", buf);
	if(strchr(buf, '-')) {
		char *start = strtok(buf, "-");
		char *end = strtok(buf, "-");
	} else if(strchr(buf, 'x')) {

	} else {
		printf("INVALID MOVE\n");
		player_move(player_color);
	}
	int *forced_moves;
	get_moves(player_color);
}
*/
int main()
{
	int board[BOARD_SIZE];
	int i;
	for(i=0; i<=19; i++) board[i] = -1;
	for(i=20; i<=29; i++) board[i] = 0;
	for(i=30; i<=49; i++) board[i] = 1;
	char buf[BUFFER_SIZE];
	printf("[White/Black]? ");
	scanf("%s", buf);
	int player_color = (buf[0] == 'W') ? 1 : -1;
	int turn = 1;

	board[21] = 1;
	get_moves(player_color, board);
	//player_move(player_color);
}
