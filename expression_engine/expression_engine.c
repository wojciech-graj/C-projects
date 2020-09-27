#include "expression_engine.h"
#include "math_functions.h"

//Possible improvement: have multiple different node structures
//multipurpose node structure. types: 'n' number, '1' L1 binary operation, '2' L2 binary operation
typedef struct Node {
	char type;
	double *val_l; //pointer to (double)val of left Node
	double *val_r;
	double (*bin_op)(double, double); //pointer to function which contains the binary operation
	double val;
} Node;

void eval_node(Node *node)
{
	double result = (node->bin_op)(*(node->val_l), *(node->val_r));
	node->val = result;
	*(node->val_l) = result;
	*(node->val_r) = result;
}

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

void set_node_type(Node *node, char *value)
{
	if(in_array(value[0], L1BINOPS, OPSLEN[0])) {
		node->type = '1';
	} else if(in_array(value[0], L2BINOPS, OPSLEN[1])){
		node->type = '2';
	} else {
		node->type = 'n';
	}
}

void assign_node_values(Node *node, double *val_l, double *val_r, double val) //double (*bin_op)(double, double)
{
	node->val_l = val_l;
	node->val_r = val_r;
	node->bin_op = NULL;
	node->val = val;
}

void set_binary_operation(Node *node, char value)
{
	switch(value) {
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
	}
}


int main()
{
	char input[] = "3 + 4 * 2";
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

	//create array of nodes and fill their parameters
	Node (*nodes)[tokens_size[0]] = malloc(1000); //TODO: change from array to linked list
	for(i = 0; i < tokens_size[0]; i++)
	{
		char *value = (*tokens)[i];
		set_node_type(nodes[i], value);
		if(nodes[i]->type == 'n') {
			assign_node_values(nodes[i], NULL, NULL, atof(value));
		}
		if(nodes[i]->type != 'n') {
			assign_node_values(nodes[i], &nodes[i-1]->val, &nodes[i+1]->val, 0);
			set_binary_operation(nodes[i], value[0]);
		}
	}

	//evaluate nodes in accordance with ORDEROPS
	Node *result_node;
	for(i = 1; i < ORDEROPSLEN; i++) //skip n, which are already evaluated
	{
		int j;
		for(j = 0; j < tokens_size[0]; j++)
		{
			if(nodes[j]->type == ORDEROPS[i]) {
				eval_node(nodes[j]);
				result_node = nodes[j];
			}
		}
	}

	double result = result_node->val;
	free(nodes);

	printf("RESULT: %f\n", result);

	return 0;
}
