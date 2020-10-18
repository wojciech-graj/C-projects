#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>

#define BOARD_SIZE 50
#define MIN_EVAL -100

const int BUFFER_SIZE = 255;
const int NEIGHBORS[] = {-5, -4, 5, 6};
const char *COLORS[] = {"White", "Black"};

const char *TITLE ="\
 ___                     _   _\n\
|   \\ _ _ __ _ _  _ __ _| |_| |_ ___\n\
| |) | '_/ _` | || / _` | ' \\  _(_-<\n\
|___/|_| \\__,_|\\_,_\\__, |_||_\\__/__/\n\
                   |___/ ";

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

int play_engine_move(int color, int *board, int remaining_depth, bool return_board, int alpha, int beta);
int create_capture_subtree(int color, int piece, int direction, int depth, int *board, Node *head);

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
