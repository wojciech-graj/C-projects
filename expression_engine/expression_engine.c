#include "expression_engine.h"
#include "math_functions.h"

//multipurpose node structure. types: 'n':number, 'c':const, '(', ')', '2':L2OP
typedef struct Node {
	char type;
	Node *node_l;
	Node *node_r;
	double (*bin_op)(double, double);
	double (*un_op)(double);
	double val;
} Node;

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
	return token_num;
}

bool in_array(char *value, const char *array[], const int length)
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
	if(in_array(value, L5OPS, OPSLEN[5])) {
		return OPTYPES[5];
	}else if(in_array(value, L4OPS, OPSLEN[4])) {
		return OPTYPES[4];
	} else if(in_array(value, L3OPS, OPSLEN[3])) {
		return OPTYPES[3];
	} else if(in_array(value, L2OPS, OPSLEN[2])) {
		return OPTYPES[2];
	} else if(in_array(value, L1OPS, OPSLEN[1])) {
		return value[0];
	} else if(in_array(value, L0OPS, OPSLEN[0])) {
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

void convert_tokens_to_nodes(Node **head, char (*tokens)[TOKEN_AMOUNT][TOKEN_LENGTH], int tokens_amount)
{
	//create doubly-linked list of blank nodes
	*head = malloc(sizeof(Node));
	Node *prev_node = *head;

	(*head)->node_l = NULL;

	int i;
	for(i = 1; i < tokens_amount; i++)
	{
		Node *cur_node = malloc(sizeof(Node));

		prev_node->node_r = cur_node;
		cur_node->node_l = prev_node;
		cur_node->un_op = NULL;
		cur_node->bin_op = NULL;

		prev_node = cur_node;
	}
	prev_node->node_r = NULL;

	//fill nodes with information from tokens
	Node *cur_node = *head;
	for(i = 0; i < tokens_amount; i++)
	{
		char *value = (*tokens)[i];
		cur_node->type = get_node_type(value);
		if(cur_node->type == 'n') {
			cur_node->val = atof(value);
		} else if(cur_node->type == 'c') {
			cur_node->val = get_math_constant(value);
		} else {
			set_operation(cur_node, value);
			cur_node->val = 0;
		}
		cur_node = cur_node->node_r;
	}
}

void delete_node(Node *node, Node **head)
{
	if(node->node_r) {
		node->node_r->node_l = node->node_l;
	}
	if(node->node_l) {
		node->node_l->node_r = node->node_r;
	} else {
		*head = node->node_r;
	}
	free(node);
}

//a node group starts with the first value, not '('
//node group e.g. (node types) {'n', '5', 'n', ')'}
//does not have to end in ')'. If it does, ')' node is deleted
void evaluate_node_group(Node **head)
{
	Node *cur_node = *head;
	Node *group_end_node = NULL;
	while(cur_node)
	{
		if(cur_node->type == '(') {
			Node *del_node = cur_node;
			cur_node = cur_node->node_r;
			delete_node(del_node, head);
			evaluate_node_group(&cur_node);
		} else if(cur_node->type == ')') {
			group_end_node = cur_node->node_l;
			delete_node(cur_node, head);
			break;
		} else if(cur_node->node_r == NULL) {
			group_end_node = cur_node;
			break;
		} else {
			cur_node = cur_node->node_r;
		}
	}

	int i;
	for(i = 2; i < NUMOPTYPES; i++) //skip L0 and L1
	{
		cur_node = *head;
		while(cur_node != group_end_node->node_r)
		{
			if(cur_node->type == OPTYPES[i]) {
				if (cur_node->bin_op) { //if binary
					cur_node->val = (cur_node->bin_op)(cur_node->node_l->val, cur_node->node_r->val);
					delete_node(cur_node->node_l, head);
					delete_node(cur_node->node_r, head);
				} else { //if unary
					if (cur_node->un_op == *factorial) { //if left-unary
						cur_node->val = (cur_node->un_op)(cur_node->node_l->val);
						delete_node(cur_node->node_l, head);
					} else { //if right-unary
						cur_node->val = (cur_node->un_op)(cur_node->node_r->val);
						delete_node(cur_node->node_r, head);
					}
				}
			}
			cur_node = cur_node->node_r;
		}
	}
}

//DOES NOT COPY POINTERS TO NEIGHBORS
void copy_node_params(Node *dest, Node *src)
{
	dest->type = src->type;
	dest->bin_op = src->bin_op;
	dest->un_op = src->un_op;
	dest->val = src->val;
}

//CURRENTLY UNUSED _ WILL BE USED FOR EVALUATING FOR X
void duplicate_nodes(Node **dest, Node *src)
{
	*dest = malloc(sizeof(Node));
	copy_node_params(*dest, src);
	(*dest)->node_l = NULL;
	Node *cur_dest_node = *dest;
	Node *cur_src_node = src->node_r;
	while(cur_src_node)
	{
		Node *node = malloc(sizeof(Node));
		copy_node_params(node, cur_src_node);
		node->node_l = cur_dest_node;
		cur_dest_node->node_r = node;
		cur_dest_node = node;

		cur_src_node = cur_src_node->node_r;
	}
	cur_dest_node->node_r = NULL;
}

int main(int argc, char *argv[])
{
	if(argc != 2)
	{
		printf("ERROR: No argument provided.\n");
		return 0;
	}

	char *input = argv[1];
	int i;

	char (*tokens)[TOKEN_AMOUNT][TOKEN_LENGTH] = malloc(TOKEN_AMOUNT * TOKEN_LENGTH * sizeof(char));
	int tokens_amount = tokenize(tokens, input);
	tokens = realloc(tokens, tokens_amount * TOKEN_LENGTH * sizeof(char)); //technically, tokens contains TOKEN_AMOUNT values, however only memory for tokens_amount has been now allocated. Size of tokens is not sizeof(tokens) but tokens_amount

	Node *head;
	convert_tokens_to_nodes(&head, tokens, tokens_amount);
	free(tokens);

	while (head->type == '(') //Delete first node if it is '(' because a node group cannot start with '('
	{
		delete_node(head, &head);
	}

	evaluate_node_group(&head);
	double result = head->val;
	free(head);

	printf("%g\n", result);

	return 0;
}
