#include "expression_engine.h"
#include "math_functions.h"

//multipurpose node structure. types: 0:number, 1:L1OPS, 2:L2OPS,
typedef struct Node {
	int type;
	Node *node_l;
	Node *node_r;
	double (*operation)(double, double);
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

//TODO:OPTIMIZE
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

void set_operation(Node *node, char *value)
{
	switch(value[0]) {
		case '+':
			node->operation = *add;
			break;
		case '-':
			node->operation = *subtract;
			break;
		case '*':
			node->operation = *multiply;
			break;
		case '/':
			node->operation = *divide;
			break;
		case '^':
			node->operation = *pow;
			break;
		case '!':
			node->operation = *factorial;
			break;
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
			cur_node->operation = NULL;
			cur_node->val = atof(value);
		}
		if(cur_node->type != 0) {
			set_operation(cur_node, value);
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
				cur_node->val = (cur_node->operation)(cur_node->node_l->val, cur_node->node_r->val);

				//delete neighboring nodes
				delete_node(cur_node->node_l, &head);
				delete_node(cur_node->node_r, &head);
			}
			cur_node = cur_node->node_r;
		}
	}

	double result = head->val;
	free(head);

	printf("%g\n", result);

	return 0;
}
