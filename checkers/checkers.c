#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>

#define BOARD_SIZE 50
#define MIN_EVAL -100

const int BUFFER_SIZE = 255;
const int NEIGHBORS[] = {-5, -4, 5, 6};

//tree node
typedef struct Node Node;
typedef struct ListNode ListNode;

int get_moves(int color, int *board, int remaining_depth, bool return_board);

typedef struct Node {
	int piece;
	int captured;
	int destination;
	Node *parent;
	Node *child;
	Node *sibling;
} Node;

typedef struct ListNode {
	ListNode *node_next;
	Node *node;
} ListNode;

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

int create_capture_tree(int color, int piece, int direction, int depth, int *board, Node *head)
{
	int max_depth = depth - 1;
	int neighbor = piece + NEIGHBORS[direction] - (int) (piece % 10 > 4); //subtract 1 every other row.
	Node *cur_sibling;
	if(! ((piece <= 9 && neighbor < piece)
		|| (piece >= 40 && neighbor > piece)
		|| (piece % 5 == 0 && direction % 2 == 0)
		|| (piece % 5 == 4 && direction % 2 == 1))) { //if not capturing over edge of board
		int behind_neighbor = neighbor + NEIGHBORS[direction] - (int)(neighbor % 10 > 4);
		if ((board[neighbor] ^ color) < 0 && board[neighbor] != 0
			&& board[behind_neighbor] == 0) { //if can capture neighbor
			Node *node;
			init_node(&node);
			if(! head->child) {
				head->child = node;
				max_depth++;
			} else {
				cur_sibling->sibling = node;
			}
			cur_sibling = node;

			node->parent = head;
			node->piece = piece;
			node->captured = neighbor;
			node->destination = behind_neighbor;

			int new_board[BOARD_SIZE];
			memcpy(new_board, board, BOARD_SIZE * sizeof(int));
			new_board[piece] = 0;
			new_board[neighbor] = 0;
			new_board[behind_neighbor] = board[piece];
			int new_direction;
			for(new_direction = 0; new_direction < 4; new_direction++)
			{
				int cur_depth = create_capture_tree(color, behind_neighbor, new_direction, depth + 1, new_board, node);
				if(cur_depth > max_depth) max_depth = cur_depth;
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
		cur_evaluation = get_moves(color * -1, board, remaining_depth - 1, false);
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

int get_moves(int color, int *board, int remaining_depth, bool return_board)
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

	int new_board[BOARD_SIZE];
	if(! head->child) {//if no captures
		for(piece = 0; piece < BOARD_SIZE; piece++)
		{
			if((board[piece] ^ color) >= 0 && board[piece] != 0) {//if piece has same sign
				int direction;
				for(direction = -1 * color + 1; direction < -1 * color + 3; direction++) //only allow forward directions
				{
					int neighbor = piece + NEIGHBORS[direction] - (int) (piece % 10 > 4); //subtract 1 every other row.
					if(! ((piece <= 4 && neighbor < piece)
						|| (piece >= 45 && neighbor > piece)
						|| (piece % 10 == 0 && direction % 2 == 0)
						|| (piece % 10 == 9 && direction % 2 == 1))) { //if not moving over edge of board
						if(board[neighbor] == 0) {
							memcpy(new_board, board, BOARD_SIZE * sizeof(int));
							new_board[piece] = 0;
							new_board[neighbor] = board[piece];
							evaluate_board(color, remaining_depth, return_board, &evaluation, new_board, best_board);
						}
					}
				}
			}
		}
	} else {
		ListNode *captures = NULL;
		get_nodes_at_depth(head, 0, max_depth, &captures);
		ListNode *cur_listhead = captures;
		while(cur_listhead)
		{
			memcpy(new_board, board, BOARD_SIZE * sizeof(int));

			Node *cur_node = cur_listhead->node;
			while(cur_node->parent)
			{
				new_board[cur_node->piece] = 0;
				new_board[cur_node->captured] = 0;
				if(! cur_node->parent->parent) {
					new_board[cur_listhead->node->destination] = board[cur_node->piece];
				}
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
	board[21] = 1;
	board[38] = 0;
	char buf[BUFFER_SIZE];
	printf("[White/Black]? ");
	scanf("%s", buf);
	int player_color = (buf[0] == 'W') ? 1 : -1;
	int turn = 1;
	print_board(board);

	int evaluation = get_moves(player_color, board, 2, true);

	print_board(board);

	printf("%d\n", evaluation);
}
