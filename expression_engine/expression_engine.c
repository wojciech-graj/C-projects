#include "expression_engine.h"
#include "math_functions.h"

//multipurpose node structure out of which tree is created. types: 'n':number, 'c':const, '(', ')', '2':L2OP
typedef struct Node {
	char type;
	Node *node_l;
	Node *node_r;
	double (*bin_op)(double, double);
	double (*un_op)(double);
	double val;
} Node;

//doubly linked list structure used when creating tree
typedef struct ListNode {
	ListNode *node_prev;
	ListNode *node_next;
	Node *node;
	bool linked; //whether or not the contained node has had its adjacent nodes defined
} ListNode;

//store tokenized input in *tokens, return amount of tokens
int tokenize(char (*tokens)[TOKEN_AMOUNT][TOKEN_LENGTH], char input[])
{
	int token_num = 0;

	char *p = strtok(input, " ");
	while(p != NULL)
	{
		strncpy((*tokens)[token_num], p, TOKEN_LENGTH);
		token_num++;
		p = strtok(NULL, " ");
	}
	tokens = realloc(tokens, token_num * TOKEN_LENGTH * sizeof(char)); //technically, tokens contains TOKEN_AMOUNT values, however only memory for tokens_amount has been now allocated. Size of tokens is not sizeof(tokens) but tokens_amount
	return token_num;
}

bool string_in_array(char *value, const char *array[], const int length)
{
	int i;
	for(i = 0; i < length; i++)
	{
		if(! strcmp(value, array[i])) return true;
	}
	return false;
}

//TODO?:OPTIMIZE
char get_node_type(char *value)
{
	if(string_in_array(value, L5OPS, OPSLEN[5])) {
		return OPTYPES[5];
	}else if(string_in_array(value, L4OPS, OPSLEN[4])) {
		return OPTYPES[4];
	} else if(string_in_array(value, L3OPS, OPSLEN[3])) {
		return OPTYPES[3];
	} else if(string_in_array(value, L2OPS, OPSLEN[2])) {
		return OPTYPES[2];
	} else if(string_in_array(value, L1OPS, OPSLEN[1])) {
		return value[0];
	} else if(string_in_array(value, L0OPS, OPSLEN[0])) {
		return OPTYPES[0];
	} else {
		return 'n';
	}
}

void set_operation(Node *node, char *value)
{
	switch(value[0]) {
		case '+':
			node->bin_op = *add;
			break;
		case '-':
			node->bin_op = *subtract;
			break;
		case '*':
			node->bin_op = *multiply;
			break;
		case '/':
			node->bin_op = *divide;
			break;
		case '%':
			node->bin_op = *modulo;
			break;
		case '^':
			node->bin_op = *pow;
			break;
		case '!':
			node->un_op = *factorial;
			break;
		default:
			if(! strcmp(value, "sqrt")) {
				node->un_op = *sqrt;
			} else if (! strcmp(value, "ln")) {
				node->un_op = *log;
			} else if (! strcmp(value, "log")) {
				node->un_op = *log10;
			} else if (! strcmp(value, "sin")) {
				node->un_op = *sin;
			} else if (! strcmp(value, "cos")) {
				node->un_op = *cos;
			} else if (! strcmp(value, "tan")) {
				node->un_op = *tan;
			} else if (! strcmp(value, "sinh")) {
				node->un_op = *sinh;
			} else if (! strcmp(value, "cosh")) {
				node->un_op = *cosh;
			} else if (! strcmp(value, "tanh")) {
				node->un_op = *tanh;
			}
	}
}

double get_math_constant(char *value)
{
	int i;
	for(i = 0; i < OPSLEN[0]; i++)
	{
		if(! strcmp(value, L0OPS[i])) return L0OPVALS[i];
	}
	return 0;
}

void delete_listnode(ListNode *node, ListNode **head, bool delete_child)
{
	if(node->node_next) {
		node->node_next->node_prev = node->node_prev;
	}
	if(node->node_prev) {
		node->node_prev->node_next = node->node_next;
	} else {
		*head = node->node_next;
	}
	if(delete_child) {
		free(node->node);
	}
	free(node);
}

bool is_left_unary(double (*un_op)(double))
{
	return un_op == *factorial;
}

void create_tree(ListNode **list_head)
{
	ListNode *cur_node = *list_head;
	ListNode *group_end_node = NULL;

	while(cur_node)
	{
		if(cur_node->node->type == '(') {
			ListNode *del_node = cur_node;
			cur_node = cur_node->node_next;
			delete_listnode(del_node, list_head, true);
			create_tree(&cur_node);
		} else if(cur_node->node->type == ')') {
			group_end_node = cur_node->node_prev;
			delete_listnode(cur_node, list_head, true);
			break;
		} else if(cur_node->node_next == NULL) {
			group_end_node = cur_node;
			break;
		} else {
			cur_node = cur_node->node_next;
		}
	}

	int i;
	for(i = 2; i < NUMOPTYPES; i++) //skip L0 and L1
	{
		cur_node = *list_head;
		while(cur_node != group_end_node->node_next)
		{
			if(cur_node->node->type == OPTYPES[i] && cur_node->linked == false) {
				cur_node->linked = true;
				if (cur_node->node->bin_op) { //if binary
					cur_node->node->node_l = cur_node->node_prev->node;
					cur_node->node->node_r = cur_node->node_next->node;
					delete_listnode(cur_node->node_prev, list_head, false);
					delete_listnode(cur_node->node_next, list_head, false);
				} else { //if unary
					if (is_left_unary(cur_node->node->un_op)) { //if left-unary
						cur_node->node->node_l = cur_node->node_prev->node;
						delete_listnode(cur_node->node_prev, list_head, false);
					} else { //if right-unary
						cur_node->node->node_r = cur_node->node_next->node;
						delete_listnode(cur_node->node_next, list_head, false);
					}
				}
			}
			cur_node = cur_node->node_next;
		}
	}
}

void create_linked_list(ListNode **list_head, int length)
{
	*list_head = malloc(sizeof(ListNode));
	(*list_head)->node_prev = NULL;

	ListNode *prev_node = *list_head;
	ListNode *cur_node;
	int i;
	for(i = 1; i < length; i++)
	{
		ListNode *cur_node = malloc(sizeof(ListNode));

		prev_node->node_next = cur_node;
		cur_node->node_prev = prev_node;

		prev_node = cur_node;
	}
	prev_node->node_next = NULL;
}

void convert_tokens_to_nodes(Node **head, char (*tokens)[TOKEN_AMOUNT][TOKEN_LENGTH], int tokens_amount)
{
	ListNode *list_head;

	create_linked_list(&list_head, tokens_amount);

	//fill linked list with nodes and those nodes with values
	ListNode *cur_node = list_head;
	int i;
	for(i = 0; i < tokens_amount; i++)
	{
		char *value = (*tokens)[i];
		Node *node = malloc(sizeof(Node));
		cur_node->node = node;
		cur_node->linked = false;
		node->node_l = NULL;
		node->node_r = NULL;

		node->type = get_node_type(value);
		if(node->type == 'n') {
			node->val = atof(value);
			node->bin_op = NULL;
			node->un_op = NULL;
		} else if(node->type == 'c') {
			node->val = get_math_constant(value);
			node->bin_op = NULL;
			node->un_op = NULL;
		} else {
			set_operation(node, value);
			node->val = 0;
		}
		cur_node = cur_node->node_next;
	}

	while (list_head->node->type == '(') //Delete first node if it is '(' because a node group cannot start with '('
	{
		delete_listnode(list_head, &list_head, true);
	}

	create_tree(&list_head);

	*head = list_head->node;
	free(list_head);
	free(tokens);
}

void substitute_variable(Node *node, char var, double val)
{
	if(node->node_l) {
		substitute_variable(node->node_l, var, val);
	}
	if(node->node_r) {
		substitute_variable(node->node_r, var, val);
	}
	if(node->type == var) {
		node->val = val;
	}
}

double evaluate_tree(Node *node)
{
	if(node->bin_op) {
		(void) evaluate_tree(node->node_l);
		(void) evaluate_tree(node->node_r);
		node->val = (node->bin_op)(node->node_l->val, node->node_r->val);
	} else if(node->un_op) {
		if (is_left_unary(node->un_op)) { //if left-unary
			(void) evaluate_tree(node->node_l);
			node->val = (node->un_op)(node->node_l->val);
		} else { //if right-unary
			(void) evaluate_tree(node->node_r);
			node->val = (node->un_op)(node->node_r->val);
		}
	}
	return node->val;
}

void delete_tree(Node *node)
{
	if(node->node_r) {
		delete_tree(node->node_r);
	}
	if(node->node_l) {
		delete_tree(node->node_l);
	}
	free(node);
}

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		printf("ERROR: No argument provided.\n");
		return 0;
	}

	char *input = argv[1];

	char (*tokens)[TOKEN_AMOUNT][TOKEN_LENGTH] = malloc(TOKEN_AMOUNT * TOKEN_LENGTH * sizeof(char));
	int tokens_amount = tokenize(tokens, input);

	Node *head;
	convert_tokens_to_nodes(&head, tokens, tokens_amount);

	if(argc > 2) {
		substitute_variable(head, 'x', atof(argv[2]));
	}
	if(argc > 3) {
		substitute_variable(head, 'y', atof(argv[3]));
	}

	double result = evaluate_tree(head);
	delete_tree(head);
	printf("%f\n", result);

	return 0;
}
