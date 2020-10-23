#ifndef CHECKERS_H
#define CHECKERS_H

#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>

#define BOARD_SIZE 50

static const int MIN_EVAL = -100;
static const int BUFFER_SIZE = 255;
static const int NEIGHBORS[] = {-5, -4, 5, 6};

#define NOT_OVER_EDGE(piece, neighbor, direction, margin)\
	(! ((piece <= 5 * margin - 1 && neighbor < piece)\
	|| (piece >= 50 - 5 * margin && neighbor > piece)\
	|| (piece % (10 / margin) == 10 - 5 * margin && direction % 2 == 0)\
	|| (piece % (10 / margin) == 4 && direction % 2 == 1)))

#define PROMOTING(type, piece) (type == -1 && piece >= 45 || type == 1 && piece <= 4)

#define NEIGHBOR_DIFF(piece, direction) (NEIGHBORS[direction] - (int) (piece % 10 > 4))

#define SAME_SIGN(a, b) ((a ^ b) >= 0)

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

typedef struct Node Node; //tree node
typedef struct ListNode ListNode;

void init_board(int *board);
void init_node(Node **node);
__attribute__((weak)) void end_game(int color);
void execute_captures(int *board, Node *node);
void execute_move(int *board, int piece, int destination);
void get_nodes_at_depth(Node *head, int depth, int target_depth, ListNode **captures);
int evaluate_board(int color, int remaining_depth, bool return_board, int *evaluation, int *board, int *best_board, int alpha, int beta);
void delete_tree(Node *node);
void delete_list(ListNode *node);
int create_capture_tree(int color, int *board, Node *head);
int play_engine_move(int color, int *board, int remaining_depth, bool return_board, int alpha, int beta);
static int create_capture_subtree(int color, int piece, int direction, int depth, int *board, Node *head);
static void append_tree(Node *head, int piece, int captured, int destination, int *board, Node **cur_sibling, int *max_depth, int color, int depth);
static bool prune(int color, int evaluation, int *alpha, int *beta);

typedef struct Node {
	int piece;
	int captured;
	int destination;
	int type;
	Node *parent;
	Node *child;
	Node *sibling;
} Node;

typedef struct ListNode {
	ListNode *node_next;
	Node *node;
} ListNode;

#endif
