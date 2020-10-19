#include "checkers.h"

void print_board(int *board)
{
	printf("   1 2 3 4 5");
	int i;
	for(i = 0; i < BOARD_SIZE * 2; i++)
	{
		char *tile;

		if(i % 20 == 10) {
			printf("\n%-2d", i / 2 + 1);
		} else if(i % 10 == 0) {
			printf("\n  ");
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

void init_node(Node **node) {
	*node = malloc(sizeof(Node));
	(*node)->child = NULL;
	(*node)->sibling = NULL;
	(*node)->parent = NULL;
}

void end_game(int color)
{
	printf("%s wins!\n", COLORS[(color + 1) / 2]);
	exit(0);
}

//node is the end node of tree
void execute_captures(int *board, Node *node)
{
	board[node->destination] = node->type;
	Node *cur_node = node;
	while(cur_node->parent)
	{
		board[cur_node->captured] = 0; //TODO: optimize
		board[cur_node->piece] = 0;
		cur_node = cur_node->parent;
	}
}

void execute_move(int *board, int piece, int destination)
{
	board[destination] = board[piece];
	board[piece] = 0;
	if(PROMOTING(board[destination], destination)) board[destination] *= 2;
}

void append_tree(Node *head, int piece, int captured, int destination, int *board, Node **cur_sibling, int *max_depth, int color, int depth) {
	Node *node;
	init_node(&node);
	if(! head->child) {
		head->child = node;
		(*max_depth)++;
	} else {
		(*cur_sibling)->sibling = node;
	}
	*cur_sibling = node;

	int new_board[BOARD_SIZE];
	memcpy(new_board, board, BOARD_SIZE * sizeof(int));
	new_board[captured] = 0;
	execute_move(new_board, piece, destination);

	node->parent = head;
	node->piece = piece;
	node->captured = captured;
	node->destination = destination;
	node->type = new_board[destination];

	int new_direction;
	for(new_direction = 0; new_direction < 4; new_direction++)
	{
		int cur_depth = create_capture_subtree(color, destination, new_direction, depth + 1, new_board, node);
		if(cur_depth > (*max_depth)) *max_depth = cur_depth;
	}
}

int create_capture_subtree(int color, int piece, int direction, int depth, int *board, Node *head)
{
	int max_depth = depth - 1;
	int captured = piece + NEIGHBOR_DIFF(piece, direction);
	if(NOT_OVER_EDGE(piece, captured, direction, 2)) {
		Node *cur_sibling;
		int destination = captured + NEIGHBOR_DIFF(captured, direction);
		if(fabs(board[piece]) == 1) { //if not queen
			if (! SAME_SIGN(board[captured], color)
				&& board[captured] != 0
				&& board[destination] == 0) { //if can capture neighbor
				append_tree(head, piece, captured, destination, board, &cur_sibling, &max_depth, color, depth);
			}
		} else { //if queen
			while(NOT_OVER_EDGE(captured, destination, direction, 1)
				&& ! (board[captured] != 0 && board[destination] != 0))
			{
				if(! SAME_SIGN(board[captured], color) && board[captured] != 0) {
					int prev_destination = captured;
					while(NOT_OVER_EDGE(prev_destination, destination, direction, 1)
						&& board[destination] == 0)
					{
						append_tree(head, piece, captured, destination, board, &cur_sibling, &max_depth, color, depth);
						prev_destination = destination;
						destination += NEIGHBOR_DIFF(destination, direction);
					}
				}
				if(board[captured] != 0) break;
				captured = destination;
				destination += NEIGHBOR_DIFF(destination, direction);
			}
		}
	}
	return max_depth;
}

void get_nodes_at_depth(Node *head, int depth, int target_depth, ListNode **captures)
{
	if(depth == target_depth) {
		ListNode *node = malloc(sizeof(ListNode));
		node->node_next = NULL;
		node->node = head;
		if(*captures) {
			ListNode *cur_listnode = *captures;
			while(cur_listnode->node_next)
			{
				cur_listnode = cur_listnode->node_next;
			}
			cur_listnode->node_next = node;
		} else {
			*captures = node;
		}
	} else if(head->child) {
		get_nodes_at_depth(head->child, depth + 1, target_depth, captures);
	}
	if(head->sibling) {
		get_nodes_at_depth(head->sibling, depth, target_depth, captures);
	}
}

int evaluate_board(int color, int remaining_depth, bool return_board, int *evaluation, int *board, int *best_board, int alpha, int beta){
	int cur_evaluation = 0;
	int piece;
	bool white_left = false;
	bool black_left = false;
	for(piece = 0; piece < BOARD_SIZE; piece++)
	{
		cur_evaluation += board[piece];
		if(black_left == false && board[piece] < 0) {
			black_left = true;
		} else if(white_left == false && board[piece] > 0) {
			white_left = true;
		}
	}
	if(! white_left) {
		cur_evaluation = MIN_EVAL;
	} else if(! black_left) {
		cur_evaluation = -MIN_EVAL;
	} else if(remaining_depth != 0) {
		cur_evaluation = play_engine_move(color * -1, board, remaining_depth - 1, false, alpha, beta);
	}
	if((color < 0 && cur_evaluation < *evaluation) || (color > 0 && cur_evaluation > *evaluation)) {
		*evaluation = cur_evaluation;
		if(return_board) {
			memcpy(best_board, board, BOARD_SIZE * sizeof(int));
		}
	}
	return cur_evaluation;
}

void delete_tree(Node *node)
{
	if(node->sibling) {
		delete_tree(node->sibling);
	}
	if(node->child) {
		delete_tree(node->child);
	}
	free(node);
}

void delete_list(ListNode *node)
{
	if(node->node_next) {
		delete_list(node->node_next);
	}
	free(node);
}

int create_capture_tree(int color, int *board, Node *head)
{
	int piece;
	int max_depth = 0;
	for(piece = 0; piece < BOARD_SIZE; piece++)
	{
		if(SAME_SIGN(board[piece], color) && board[piece] != 0) {
			int direction;
			for(direction = 0; direction < 4; direction++)
			{
				int depth = create_capture_subtree(color, piece, direction, 1, board, head);
				if(depth > max_depth) max_depth = depth;
			}
		}
	}
	return max_depth;
}

bool prune(int color, int evaluation, int *alpha, int *beta)
{
	if(color == 1) {
		*alpha = MAX(*alpha, evaluation);
	} else if(color == -1) {
		*beta = MIN(*beta, evaluation);
	}
	return (*beta <= *alpha);
}

int play_engine_move(int color, int *board, int remaining_depth, bool return_board, int alpha, int beta)
{
	Node *head;
	init_node(&head);
	int max_depth = create_capture_tree(color, board, head);

	int evaluation = 2 * MIN_EVAL * color;
	int best_board[BOARD_SIZE];

	int piece;
	int new_board[BOARD_SIZE];
	if(! head->child) {//if no captures
		bool game_over = true;
		for(piece = 0; piece < BOARD_SIZE; piece++)
		{
			if(SAME_SIGN(board[piece], color) && board[piece] != 0) {//if piece has same sign
				int direction;
				if(fabs(board[piece]) == 1) {//if not queen
					for(direction = -color + 1; direction < -color + 3; direction++) //only allow forward directions
					{
						int neighbor = piece + NEIGHBOR_DIFF(piece, direction);
						if(NOT_OVER_EDGE(piece, neighbor, direction, 1)) {
							if(board[neighbor] == 0) {
								if(game_over == true) game_over = false;
								memcpy(new_board, board, BOARD_SIZE * sizeof(int));
								execute_move(new_board, piece, neighbor);
								int cur_evaluation = evaluate_board(color, remaining_depth, return_board, &evaluation, new_board, best_board, alpha, beta);
								if(prune(color, cur_evaluation, &alpha, &beta)) goto END;

							}
						}
					}
				} else {//if queen
					for(direction = 0; direction < 4; direction++)
					{
						int neighbor = piece + NEIGHBOR_DIFF(piece, direction);
						int prev_neighbor = piece;
						while(NOT_OVER_EDGE(prev_neighbor, neighbor, direction, 1)) {
							if(board[neighbor] == 0) {
								if(game_over == true) game_over = false;
								memcpy(new_board, board, BOARD_SIZE * sizeof(int));
								execute_move(new_board, piece, neighbor);
								int cur_evaluation = evaluate_board(color, remaining_depth, return_board, &evaluation, new_board, best_board, alpha, beta);

								if(prune(color, cur_evaluation, &alpha, &beta)) goto END;
							} else {
								break;
							}
							prev_neighbor = neighbor;
							neighbor += NEIGHBOR_DIFF(neighbor, direction);
						}
					}
				}
			}
		}
		if(game_over == true) {
			if(return_board) {
				end_game(color);
			} else {
				evaluation = color * MIN_EVAL;
			}
		}
	} else { //if forced captures
		ListNode *captures = NULL;
		get_nodes_at_depth(head, 0, max_depth, &captures);
		ListNode *cur_listhead = captures;
		while(cur_listhead)
		{
			memcpy(new_board, board, BOARD_SIZE * sizeof(int));
			execute_captures(new_board, cur_listhead->node);
			int cur_evaluation = evaluate_board(color, remaining_depth, return_board, &evaluation, new_board, best_board, alpha, beta);

			if(prune(color, cur_evaluation, &alpha, &beta)) {
				delete_list(captures);
				goto END;
			}
			cur_listhead = cur_listhead->node_next;
		}
		delete_list(captures);
	}
	END:
	delete_tree(head);
	if(return_board) {
		memcpy(board, best_board, BOARD_SIZE * sizeof(int));
	}
	return evaluation;
}

//TODO: add help text
void play_player_move(int color, int *board)
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
	int i;

	for(i=0; i<=19; i++) board[i] = -1;
	for(i=20; i<=29; i++) board[i] = 0;
	for(i=30; i<=49; i++) board[i] = 1;

	printf("%s\n", TITLE);

	char buf[BUFFER_SIZE];
	char players[2];
	int computer_depth[2];
	for(i = 0; i <= 1; i++)
	{
		GET_PLAYER:
		printf("%s: [CPU/Player]? ", COLORS[i]);
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
