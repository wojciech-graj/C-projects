#include "checkers.h"

void init_board(int *board)
{
	int i;
	for(i=0; i<=19; i++) board[i] = -1;
	for(i=20; i<=29; i++) board[i] = 0;
	for(i=30; i<=49; i++) board[i] = 1;
}

void init_node(Node **node) {
	*node = malloc(sizeof(Node));
	(*node)->child = NULL;
	(*node)->sibling = NULL;
	(*node)->parent = NULL;
}

void end_game(int color)
{
	printf("%s wins!\n", PLAYER_COLORS[(color + 1) / 2]);
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

int evaluate_board(int color, int remaining_depth, bool return_board, int *evaluation, int *board, int *best_board, int alpha, int beta) {
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
