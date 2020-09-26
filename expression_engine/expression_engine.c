#include "expression_engine.h"
#include "math_functions.h"

//Possible improvement: have multiple different node structures
//multipurpose node structure. types: 'n' number, 'b' binary operation
typedef struct Node {
	char type;
	double *val_l; //pointer to (double)val of left Node
	double *val_r;
	double (*bin_op)(double, double); //pointer to function which contains the binary operation
	double val;
} Node;

void eval_binary(Node *node)
{
	node->val = (node->bin_op)(*(node->val_l), *(node->val_r));
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

bool in_array(char value, const char *array)
{
	int i;
	for(i = 0; i < sizeof(array) / sizeof(char); i++)
	{
		if(value == array[i]) return true;
	}
	return false;
}

char get_node_type(char *value)
{
	if(in_array(value[0], OPERATIONS)) {
		return 'b';
	} else {
		return 'n';
	}
}

int main()
{
	char input[] = "1 + 22";
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

	//create array of nodes and TODO:fill their parameters
	Node (*nodes)[tokens_size[0]] = malloc(tokens_size[0] * sizeof(Node));
	for(i = 0; i < tokens_size[0]; i++)
	{
		nodes[i]->type = get_node_type((*tokens)[i]);
	}

	return 0;
}
