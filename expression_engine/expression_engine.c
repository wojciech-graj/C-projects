#include "expression_engine.h"
#include "math_functions.h"

//Possible improvement: have multiple different node structures
//multipurpose node structure. types: 0:number, 1:L1OPS, 2:L2OPS,
typedef struct Node {
	int type;
	Node *node_l;
	Node *node_r;
	double (*bin_op)(double, double);
	double val;
} Node;

void tokenize(char (*tokens)[TOKEN_AMOUNT][TOKEN_LENGTH], char input[])
{
	int i = 0;

	char *p = strtok(input, " ");
	while(p != NULL)
	{
		strncpy((*tokens)[i], p, TOKEN_LENGTH);
		i++;
		p = strtok(NULL, " ");
	}
}

//set tokens_size to (length, token size). tokens_size[0] must = 0
void get_tokens_size(char (*tokens)[TOKEN_AMOUNT][TOKEN_LENGTH], int *tokens_size)
{
	while((*tokens)[tokens_size[0]][0])
	{
		int len = strlen((*tokens)[tokens_size[0]]);
		if (len > tokens_size[1]) tokens_size[1] = len;
		tokens_size[0]++;
	}
}

bool in_array(char value, const char *array, const int length)
{
	int i;
	for(i = 0; i < length; i++)
	{
		if(value == array[i]) return true;
	}
	return false;
}

int get_node_type(char *value)
{
	if(in_array(value[0], L5OPS, OPSLEN[5])) {
		return 5;
	}else if(in_array(value[0], L4OPS, OPSLEN[4])) {
		return 4;
	} else if(in_array(value[0], L3OPS, OPSLEN[3])){
		return 3;
	} else if(in_array(value[0], L2OPS, OPSLEN[2])){
		return 2;
	} else {
		return 0;
	}
}


void set_binary_operation(Node *node, char *value)
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
		case '^':
			node->bin_op = *pow;
			break;
	}
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

	//get tokens from input and store in smallest possible array
	char (*tokensbuf)[TOKEN_AMOUNT][TOKEN_LENGTH] = malloc(TOKEN_AMOUNT * TOKEN_LENGTH * sizeof(char));
	tokenize(tokensbuf, input);
	int tokens_size[] = {0,0};
	get_tokens_size(tokensbuf, tokens_size);
	tokens_size[1] += 1; //add space for '\0'
	char (*tokens)[tokens_size[0]][tokens_size[1]] = malloc(tokens_size[0] * tokens_size[1] * sizeof(char));
	for(i = 0; i < tokens_size[0]; i++)
	{
		strncpy((*tokens)[i], (*tokensbuf)[i], tokens_size[1]);
	}
	free(tokensbuf);

	//create double linked list of nodes
	Node *head = malloc(sizeof(Node));
	Node *prev_node = head;

	head->node_l = NULL;

	for(i = 1; i < tokens_size[0]; i++)
	{
		Node *cur_node = malloc(sizeof(Node));

		prev_node->node_r = cur_node;
		cur_node->node_l = prev_node;
		cur_node->node_r = NULL;

		prev_node = cur_node;
	}

	//fill parameters
	Node *cur_node = head;
	for(i = 0; i < tokens_size[0]; i++)
	{
		char *value = (*tokens)[i];
		cur_node->type = get_node_type(value);
		if(cur_node->type == 0) {
			cur_node->bin_op = NULL;
			cur_node->val = atof(value);
		}
		if(cur_node->type != 0) {
			set_binary_operation(cur_node, value);
			cur_node->val = 0;
		}
		cur_node = cur_node->node_r;
	}

	//evaluate nodes in accordance with order of operations
	for(i = 2; i <= NUMLEVELS; i++) //skip L0 and L1
	{
		cur_node = head;
		while(cur_node)
		{
			if(cur_node->type == i) {
				cur_node->val = (cur_node->bin_op)(cur_node->node_l->val, cur_node->node_r->val);

				//delete neighboring nodes
				Node *del_node = cur_node->node_r;
				if(cur_node->node_r->node_r) {
					cur_node->node_r = cur_node->node_r->node_r;
					cur_node->node_r->node_l = cur_node;
				} else {
					cur_node->node_r = NULL;
				}
				free(del_node);

				del_node = cur_node->node_l;
				if(cur_node->node_l->node_l) {
					cur_node->node_l = cur_node->node_l->node_l;
					cur_node->node_l->node_r = cur_node;
				} else {
					cur_node->node_l = NULL;
					head = cur_node;
				}
				free(del_node);
			}
			cur_node = cur_node->node_r;
		}
	}

	double result = head->val;
	free(head);

	printf("%g\n", result);

	return 0;
}
