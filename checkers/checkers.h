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

int play_engine_move(int color, int *board, int remaining_depth, bool return_board);

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
