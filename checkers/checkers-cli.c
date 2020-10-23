#include "checkers-cli.h"

void end_game(int color)
{
	printf("%s wins!\n", PLAYER_COLORS[(color + 1) / 2]);
	exit(0);
}

static void print_board(int *board)
{
	printf("    1 2 3 4 5\n  ╔══════════╗");
	int i;
	for(i = 0; i < BOARD_SIZE * 2; i++)
	{
		char *tile;

		if(i % 20 == 10) {
			printf("║\n%-2d║", i / 2 + 1);
		} else if(i % 10 == 0) {
			if(i != 0) printf("║");
			printf("\n  ║");
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
			tile = "█";
		}
		printf("%s", tile);
	}
	printf("║\n  ╚══════════╝\n");
}

static void play_player_move(int color, int *board)
{
	//check if there exist any moves
	int new_board[BOARD_SIZE];
	memcpy(new_board, board, BOARD_SIZE * sizeof(int));
	(void) play_engine_move(color, new_board, 0, true, -MIN_EVAL, MIN_EVAL);

	Node *head;
	init_node(&head);
	int max_depth = create_capture_tree(color, board, head);
	ListNode *captures = NULL;
	if(head->child) get_nodes_at_depth(head, 0, max_depth, &captures);
	char buf[BUFFER_SIZE];

	GET_INPUT:
	printf(": ");
	scanf("%s", buf);
	strtok(buf, "\n");
	if(! strcmp(buf, "help")) {
		printf("%s", HELP);
		goto GET_INPUT;
	} else if(! strcmp(buf, "capture")) {
		if(! head->child) {
			printf("ERROR: NO POSSIBLE CAPTURES\n");
			goto GET_INPUT;
		} else if(captures->node_next) {
			printf("ERROR: MUTLIPLE POSSIBLE CAPTURES\n");
			goto GET_INPUT;
		}
		execute_captures(board, captures->node);
		delete_list(captures);
	} else if(! strcmp(buf, "resign")) {
		if(head->child) delete_list(captures);
		delete_tree(head);
		end_game(color);
	} else if(strchr(buf, '-')) {
		if(head->child) {
			printf("ERROR: CAPTURES ARE POSSIBLE\n");
			goto GET_INPUT;
		}
		int piece = atoi(strtok(buf, "-")) - 1;
		int destination = atoi(strtok(NULL, "-")) - 1;
		if(piece == destination) {
			printf("ERROR: ORIGIN OF PIECE IS SAME AS DESTINATION\n");
			goto GET_INPUT;
		} else if(MIN(piece, destination) < 0 || MAX(piece, destination) > 49) {
			printf("ERROR: MOVE IS NOT WITHIN BOUNDS OF BOARD\n");
			goto GET_INPUT;
		}
		int difference = destination - piece;
		if(! SAME_SIGN(board[piece], color) || board[piece] == 0) {
			printf("ERROR: PIECE WHICH IS BEING MOVED DOES NOT BELONG TO YOU\n");
			goto GET_INPUT;
		} else if(board[destination] != 0) {
			printf("ERROR: DESTINATION SQUARE IS OCCUPIED\n");
			goto GET_INPUT;
		}
		if(fabs(board[piece]) == 1) { //if not queen
			difference += (int) (piece % 10 > 4);
			int direction = 0;
			while(direction <= 3 && NEIGHBORS[direction] != difference) direction++;
			if(direction == 4) {
				printf("ERROR: PIECE IS NOT MOVING DIAGONALLY BY 1 SQUARE\n");
				goto GET_INPUT;
			}
			if(SAME_SIGN(difference, color)) {
				printf("ERROR: CANNOT MOVE BACKWARDS\n");
				goto GET_INPUT;
			} else if(! NOT_OVER_EDGE(piece, destination, direction, 1)) {
				printf("ERROR: CANNOT MOVE OVER EDGE OF BOARD\n");
				goto GET_INPUT;
			} else {
				execute_move(board, piece, destination);
			}
		} else { //if queen
			int direction;
			for(direction = 2 * (int) (difference > 0); direction < 2 + 2 * (int) (difference > 0); direction++) //only check directions with same sign as difference, either 0,1 or 2,3
			{
				int temp_piece = piece;
				int next_temp_piece = piece + NEIGHBOR_DIFF(piece, direction);
				while(NOT_OVER_EDGE(temp_piece, next_temp_piece, direction, 1) && board[next_temp_piece] == 0)
				{
					if(next_temp_piece == destination) {
						execute_move(board, piece, destination);
						goto END;
					}
					temp_piece = next_temp_piece;
					next_temp_piece += NEIGHBOR_DIFF(next_temp_piece, direction);
				}
			}
			printf("ERROR: THIS MOVE CANNOT BE PERFORMED\n");
			goto GET_INPUT;
		}
	} else if(strchr(buf, 'x')) { //if capturing
		if(! head->child) {
			printf("ERROR: NO POSSIBLE CAPTURES\n");
			goto GET_INPUT;
		}
		int moves[BUFFER_SIZE];
		int i = 0;
		char *token = strtok(buf, "x");
		while(token)
		{
			moves[i] = atoi(token) - 1;
			if(moves[i] < 0 || moves[i] > 49) {
				printf("ERROR: MOVE IS NOT WITHIN BOUNDS OF BOARD\n");
				goto GET_INPUT;
			}
			i++;
			token = strtok(NULL, "x");
		}
		if(i - 1 != max_depth){
			printf("ERROR: LENGTH OF CAPTURE SEQUENCE IS INCORRECT");
			goto GET_INPUT;
		}
		ListNode *cur_listhead = captures;
		while(cur_listhead)
		{
			Node *cur_node = cur_listhead->node;
			for(i = max_depth; i >= 0; i--)
			{
				if(cur_node->destination != moves[i]) break;
				if(i == 1 && cur_node->piece == moves[0]) {
					execute_captures(board, captures->node);
					delete_list(captures);
					goto END;
				}
				cur_node = cur_node->parent;
			}
			cur_listhead = cur_listhead->node_next;
		}
		printf("ERROR: THIS CAPTURE CANNOT BE PERFORMED\n");
		goto GET_INPUT;
	} else {
		printf("ERROR: INVALID INPUT\n");
		goto GET_INPUT;
	}
	END:
	delete_tree(head);
}

int main()
{
	int board[BOARD_SIZE];

	init_board(board);

	printf("%s\n", TITLE);

	char buf[BUFFER_SIZE];
	char players[2];
	int computer_depth[2];
	int i;
	for(i = 0; i <= 1; i++)
	{
		GET_PLAYER:
		printf("%s: [CPU/Player]? ", PLAYER_COLORS[i]);
		scanf("%s", buf);
		if(buf[0] == 'C') {
			players[i] = buf[0];
			GET_DEPTH:
			printf("depth: [>0]? ");
			scanf("%s", buf);
			computer_depth[i] = atoi(buf);
			if(computer_depth[i] < 1) {
				printf("INVALID INPUT\n");
				goto GET_DEPTH;
			}
		} else if(buf[0] == 'P') {
			players[i] = buf[0];
		} else {
			printf("INVALID INPUT\n");
			goto GET_PLAYER;
		}
	}

	print_board(board);
	int color = 1;
	while(true)
	{
		if(players[i % 2] == 'C') {
			(void) play_engine_move(color, board, computer_depth[i % 2], true, MIN_EVAL, -MIN_EVAL);
		} else if(players[i % 2] == 'P') {
			play_player_move(color, board);
		}
		printf("\n");
		print_board(board);
		color *= -1;
		i += 1;
	}
}
