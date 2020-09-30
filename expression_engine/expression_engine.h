#ifndef _EXPRESSION_ENGINE_H_
#define _EXPRESSION_ENGINE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

typedef struct Node Node;
typedef struct ListNode ListNode;

static const char *L5OPS[] = {"+", "-"};
static const char *L4OPS[] = {"/", "*", "%"};
static const char *L3OPS[] = {"^"};
static const char *L2OPS[] = {"!", "ln", "log", "sqrt", "sin", "cos", "tan", "sinh", "cosh", "tanh"};
static const char *L1OPS[] = {"(", ")", "x", "y"};
static const char *L0OPS[] = {"pi", "e", "phi"};
static const int OPSLEN[] = {3, 4, 10, 1, 3, 2};
static const double L0OPVALS[] = {M_PI, M_E, (1+sqrt(5))/2};
static const char OPTYPES[] = {'c', '\0', '2', '3', '4', '5'};
static const int NUMOPTYPES = 6;

static const int TOKEN_LENGTH = 6;
static const int TOKEN_AMOUNT = 30;

int tokenize(char (*tokens)[TOKEN_AMOUNT][TOKEN_LENGTH], char input[]);
bool string_in_array(char *value, const char *array[], const int length);
char get_node_type(char *value);
void set_operation(Node *node, char *value);
double get_math_constant(char *value);
void delete_listnode(ListNode *node, ListNode **head, bool delete_child);
bool is_left_unary(double (*un_op)(double));
void create_tree(ListNode **list_head);
void create_linked_list(ListNode **list_head, int length);
void convert_tokens_to_nodes(Node **head, char (*tokens)[TOKEN_AMOUNT][TOKEN_LENGTH], int tokens_amount);
void substitute_variable(Node *node, char var, double val);
double evaluate_tree(Node *node);
void delete_tree(Node *node);

#endif
