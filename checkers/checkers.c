#include "checkers.h"

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

void init_node(Node **node) {
	*node = malloc(sizeof(Node));
	(*node)->child = NULL;
	(*node)->sibling = NULL;
	(*node)->parent = NULL;
}

void append_tree(Node *head, int piece, int neighbor, int behind_neighbor, int *board, Node **cur_sibling, int *max_depth, int color, int depth) {
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
	new_board[piece] = 0;
	new_board[neighbor] = 0;
	new_board[behind_neighbor] = board[piece];

	if(PROMOTING(board[piece], behind_neighbor)) new_board[behind_neighbor] *= 2;

	node->parent = head;
	node->piece = piece;
	node->captured = neighbor;
	node->destination = behind_neighbor;
	node->type = new_board[behind_neighbor];

	int new_direction;
	for(new_direction = 0; new_direction < 4; new_direction++)
	{
		int cur_depth = create_capture_subtree(color, behind_neighbor, new_direction, depth + 1, new_board, node);
		if(cur_depth > (*max_depth)) *max_depth = cur_depth;
	}
}

int create_capture_subtree(int color, int piece, int direction, int depth, int *board, Node *head)
{
	int max_depth = depth - 1;
	int neighbor = piece + NEIGHBORS[direction] - (int) (piece % 10 > 4);
	if(NOT_OVER_EDGE(piece, neighbor, direction, 2)) {
		Node *cur_sibling;
		int behind_neighbor = neighbor + NEIGHBORS[direction] - (int)(neighbor % 10 > 4);
		if(fabs(board[piece]) == 1) { //if not queen
			if ((board[neighbor] ^ color) < 0 && board[neighbor] != 0
				&& board[behind_neighbor] == 0) { //if can capture neighbor
				append_tree(head, piece, neighbor, behind_neighbor, board, &cur_sibling, &max_depth, color, depth);
			}
		} else { //if queen
			while(NOT_OVER_EDGE(neighbor, behind_neighbor, direction, 1)
				&& ! (board[neighbor] != 0 && board[behind_neighbor] != 0))
			{
				if((board[neighbor] ^ color) < 0 && board[neighbor] != 0) {
					int prev_behind_neighbor = neighbor;
					while(NOT_OVER_EDGE(prev_behind_neighbor, behind_neighbor, direction, 1)
						&& board[behind_neighbor] == 0)
					{
						append_tree(head, piece, neighbor, behind_neighbor, board, &cur_sibling, &max_depth, color, depth);
						prev_behind_neighbor = behind_neighbor;
						behind_neighbor += NEIGHBORS[direction] - (int)(behind_neighbor % 10 > 4);
					}
				}
				if(board[neighbor] != 0) break;
				neighbor = behind_neighbor;
				behind_neighbor += NEIGHBORS[direction] - (int)(behind_neighbor % 10 > 4);
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

void evaluate_board(int color, int remaining_depth, bool return_board, int *evaluation, int *board, int *best_board){
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
		cur_evaluation = -1 * MIN_EVAL;
	} else if(remaining_depth != 0) {
		cur_evaluation = play_engine_move(color * -1, board, remaining_depth - 1, false);
	}
	if((color < 0 && cur_evaluation < *evaluation) || (color > 0 && cur_evaluation > *evaluation)) {
		*evaluation = cur_evaluation;
		if(return_board) {
			memcpy(best_board, board, BOARD_SIZE * sizeof(int));
		}
	}
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
		if((board[piece] ^ color) >= 0 && board[piece] != 0) {//if piece has same sign
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

int play_engine_move(int color, int *board, int remaining_depth, bool return_board)
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
			if((board[piece] ^ color) >= 0 && board[piece] != 0) {//if piece has same sign
				int direction;
				if(fabs(board[piece]) == 1) {//if not queen
					for(direction = -1 * color + 1; direction < -1 * color + 3; direction++) //only allow forward directions
					{
						int neighbor = piece + NEIGHBORS[direction] - (int) (piece % 10 > 4);
						if(NOT_OVER_EDGE(piece, neighbor, direction, 1)) {
							if(board[neighbor] == 0) {
								if(game_over == true) game_over = false;
								memcpy(new_board, board, BOARD_SIZE * sizeof(int));
								new_board[piece] = 0;
								new_board[neighbor] = board[piece];
								if(PROMOTING(board[piece], neighbor)) new_board[neighbor] *= 2;
								evaluate_board(color, remaining_depth, return_board, &evaluation, new_board, best_board);
							}
						}
					}
				} else {//if queen
					for(direction = 0; direction < 4; direction++)
					{
						int neighbor = piece + NEIGHBORS[direction] - (int) (piece % 10 > 4);
						int prev_neighbor = piece;
						while(NOT_OVER_EDGE(prev_neighbor, neighbor, direction, 1)) {
							if(board[neighbor] == 0) {
								if(game_over == true) game_over = false;
								memcpy(new_board, board, BOARD_SIZE * sizeof(int));
								new_board[piece] = 0;
								new_board[neighbor] = board[piece];
								if(PROMOTING(board[piece], neighbor)) new_board[neighbor] *= 2;
								evaluate_board(color, remaining_depth, return_board, &evaluation, new_board, best_board);
							} else {
								break;
							}
							prev_neighbor = neighbor;
							neighbor += NEIGHBORS[direction] - (int) (neighbor % 10 > 4);
						}
					}
				}
			}
		}
		if(game_over == true) {
			if(return_board) {
				if(color == 1) {
					printf("Black is the winner!\n");
				} else {
					printf("White is the winner!\n");
				}
				exit(0);
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

			Node *cur_node = cur_listhead->node;
			new_board[cur_node->destination] = cur_node->type;
			while(cur_node->parent)
			{
				new_board[cur_node->piece] = 0;
				new_board[cur_node->captured] = 0;

				cur_node = cur_node->parent;
			}

			evaluate_board(color, remaining_depth, return_board, &evaluation, new_board, best_board);

			cur_listhead = cur_listhead->node_next;
		}
		delete_list(captures);
	}
	delete_tree(head);
	if(return_board) {
		memcpy(board, best_board, BOARD_SIZE * sizeof(int));
	}
	return evaluation;
}

void play_player_move(int color, int *board)
{
	//TODO: add support for: resign, capture
	//TODO: check for moves / loss
	Node *head;
	init_node(&head);
	int max_depth = create_capture_tree(color, board, head);
	ListNode *captures = NULL;
	if(head->child) get_nodes_at_depth(head, 0, max_depth, &captures);
	char buf[BUFFER_SIZE];

	GET_INPUT:
	printf("Move: ");
	scanf("%s", buf);
	strtok(buf, "\n");
	if(strchr(buf, '-') && ! head->child) {
		int piece = atoi(strtok(buf, "-")) - 1;
		int destination = atoi(strtok(NULL, "-")) - 1;
		if(piece == destination) goto INVALID_MOVE;
		int difference = destination - piece;
		if((board[piece] ^ color) >= 0 && board[piece] != 0 && board[destination] == 0) {
			if(fabs(board[piece]) == 1) { //if not queen
				difference += (int) (piece % 10 > 4);
				int direction = 0;
    			while(direction <= 3 && NEIGHBORS[direction] != difference) direction++;
				if(direction == 4) goto INVALID_MOVE;
				if((difference ^ color) < 0
					&& NOT_OVER_EDGE(piece, destination, direction, 1)
					&& (direction + color == 1 || direction + color == 2)) {
					board[destination] = board[piece];
					if(PROMOTING(board[piece], destination)) board[destination] *= 2;
					board[piece] = 0;
				} else {
					goto INVALID_MOVE;
				}
			} else { //if queen
				int direction;
				for(direction = 2 * (int) (difference > 0); direction < 2 + 2 * (int) (difference > 0); direction++) //only check directions with same sign as difference, either 0,1 or 2,3
				{
					int temp_piece = piece;
					int next_temp_piece = piece + NEIGHBORS[direction] - (int) (piece % 10 > 4);
					while(NOT_OVER_EDGE(temp_piece, next_temp_piece, direction, 1) && board[next_temp_piece] == 0)
					{
						if(next_temp_piece == destination) {
							board[destination] = board[piece];
							if(PROMOTING(board[piece], destination)) board[destination] *= 2;
							board[piece] = 0;
							goto INPUT_END;
						}
						temp_piece = next_temp_piece;
						next_temp_piece += NEIGHBORS[direction] - (int) (next_temp_piece % 10 > 4);
					}
				}
				goto INVALID_MOVE;
			}
		} else {
			goto INVALID_MOVE;
		}
	} else if(strchr(buf, 'x') && head->child) { //if capturing
		int moves[BUFFER_SIZE];
		int i = 0;
		char *token = strtok(buf, "x");
		while(token)
		{
			moves[i] = atoi(token) - 1;
			i++;
			token = strtok(NULL, "x");
		}
		if(i - 1 != max_depth) goto INVALID_MOVE;
		ListNode *cur_listhead = captures;
		while(cur_listhead)
		{
			Node *cur_node = cur_listhead->node;
			for(i = max_depth; i >= 0; i--)
			{
				if(cur_node->destination != moves[i]) break;
				if(i == 1 && cur_node->piece == moves[0]) {
					board[cur_listhead->node->destination] = cur_listhead->node->type;
					board[cur_node->piece] = 0;
					cur_node = cur_listhead->node;
					while(cur_node->parent)
					{
						board[cur_node->captured] = 0;
						cur_node = cur_node->parent;
					}
					delete_list(captures);
					goto INPUT_END;
				}
				cur_node = cur_node->parent;
			}
			cur_listhead = cur_listhead->node_next;
		}
		goto INVALID_MOVE;
	} else {
		INVALID_MOVE:
		printf("INVALID MOVE\n");
		goto GET_INPUT;
	}
	INPUT_END:
	delete_tree(head);
}

int main()
{
	int board[BOARD_SIZE];
	int i;

	const int mode = 0;

	for(i=0; i<=19; i++) board[i] = -1;
	for(i=20; i<=29; i++) board[i] = 0;
	for(i=30; i<=49; i++) board[i] = 1;

	if(mode == 0) {
		char buf[BUFFER_SIZE];
		printf("[White/Black]? ");
		scanf("%s", buf);
		int player_color = (buf[0] == 'W') ? 1 : -1;
		int turn = 1;
		print_board(board);
		while(true)
		{
			if(turn == player_color) {
				play_player_move(turn, board);
			} else {
				(void) play_engine_move(turn, board, 6, true);
			}
			print_board(board);
			turn *= -1;
		}
	} else if(mode == 1) {
		for(i = 0; i < 200; i++)
		{
			print_board(board);
			(void) play_engine_move(1, board, 6, true);
			print_board(board);
			(void) play_engine_move(-1, board, 4, true);
		}
		print_board(board);
	}
}
