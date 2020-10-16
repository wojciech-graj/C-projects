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

	if(fabs(board[piece]) == 1 && (behind_neighbor >= 45 || behind_neighbor <= 4)) {//promote to queen
		new_board[behind_neighbor] *= 2;
	}

	node->parent = head;
	node->piece = piece;
	node->captured = neighbor;
	node->destination = behind_neighbor;
	node->type = new_board[behind_neighbor];

	int new_direction;
	for(new_direction = 0; new_direction < 4; new_direction++)
	{
		int cur_depth = create_capture_tree(color, behind_neighbor, new_direction, depth + 1, new_board, node);
		if(cur_depth > (*max_depth)) *max_depth = cur_depth;
	}
}

int create_capture_tree(int color, int piece, int direction, int depth, int *board, Node *head)
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
	if(remaining_depth == 0) {
		for(piece = 0; piece < BOARD_SIZE; piece++)
		{
			cur_evaluation += board[piece];
		}
	} else {
		cur_evaluation = play_engine_move(color * -1, board, remaining_depth - 1, false);
	}
	if((color < 0 && cur_evaluation < *evaluation) || (color > 0 && cur_evaluation > *evaluation)) { //TODO:OPTIMIZE
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

int play_engine_move(int color, int *board, int remaining_depth, bool return_board)
{
	Node *head;
	init_node(&head);

	//calculate forced moves
	int piece;
	int max_depth = 0;
	for(piece = 0; piece < BOARD_SIZE; piece++)
	{
		if((board[piece] ^ color) >= 0 && board[piece] != 0) {//if piece has same sign
			int direction;
			for(direction = 0; direction < 4; direction++)
			{
				int depth = create_capture_tree(color, piece, direction, 1, board, head);
				if(depth > max_depth) max_depth = depth;
			}
		}
	}

	int evaluation = MIN_EVAL * color;
	int best_board[BOARD_SIZE];
	//TODO:FUNCTION
	int new_board[BOARD_SIZE];
	if(! head->child) {//if no captures
		for(piece = 0; piece < BOARD_SIZE; piece++)
		{
			if((board[piece] ^ color) >= 0 && board[piece] != 0) {//if piece has same sign
				int direction;
				if(fabs(board[piece]) == 1) {//if not queen
					for(direction = -1 * color + 1; direction < -1 * color + 3; direction++) //only allow forward directions
					{
						int neighbor = piece + NEIGHBORS[direction] - (int) (piece % 10 > 4); //subtract 1 every other row.
						if(NOT_OVER_EDGE(piece, neighbor, direction, 1)) {
							if(board[neighbor] == 0) {
								memcpy(new_board, board, BOARD_SIZE * sizeof(int));
								new_board[piece] = 0;
								new_board[neighbor] = board[piece];
								if(fabs(board[piece]) == 1 && (neighbor >= 45 || neighbor <= 4)) {//promote to queen TODO:MACRO
									new_board[neighbor] *= 2;
								}
								evaluate_board(color, remaining_depth, return_board, &evaluation, new_board, best_board);
							}
						}
					}
				} else {//if queen
					for(direction = 0; direction < 4; direction++)
					{
						int prev_neighbor = piece;
						int neighbor = prev_neighbor + NEIGHBORS[direction] - (int) (prev_neighbor % 10 > 4);
						while(NOT_OVER_EDGE(prev_neighbor, neighbor, direction, 1)) {
							if(board[neighbor] == 0) {
								memcpy(new_board, board, BOARD_SIZE * sizeof(int));
								new_board[piece] = 0;
								new_board[neighbor] = board[piece];
								if(fabs(board[piece]) == 1 && (neighbor >= 45 || neighbor <= 4)) {//promote to queen TODO:MACRO
									new_board[neighbor] *= 2;
								}
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

int main()
{
	int board[BOARD_SIZE];
	int i;

	for(i=0; i<=19; i++) board[i] = -1;
	for(i=20; i<=29; i++) board[i] = 0;
	for(i=30; i<=49; i++) board[i] = 1;

	/*
	char buf[BUFFER_SIZE];
	printf("[White/Black]? ");
	scanf("%s", buf);
	int player_color = (buf[0] == 'W') ? 1 : -1;
	int turn = 1;

	board[21] = 1;
	board[38] = 0;

	print_board(board);

	int evaluation = play_engine_move(player_color, board, 2, true);

	print_board(board);

	printf("%d\n", evaluation);
	*/

	for(i = 0; i < 60; i++)
	{
		print_board(board);
		(void) play_engine_move(1, board, 6, true);
		print_board(board);
		(void) play_engine_move(-1, board, 2, true);
	}
	print_board(board);



}
