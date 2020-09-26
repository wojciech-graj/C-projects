#include "linked_lists.h"

typedef struct Node {
	int data;
	Node *next;
} Node;

void file_write(Node *list, char filename[])
{
	FILE *file = fopen(filename, "w+");

	Node *cur_node = list;
	while(cur_node->next)
	{
		fwrite(&(cur_node->data), sizeof(int), 1, file);
		cur_node = cur_node->next;
	}
	fwrite(&(cur_node->data), sizeof(int), 1, file);

	fclose(file);
}

Node *file_read(char filename[])
{
	FILE *file = fopen(filename, "r");
	Node *list = malloc(sizeof(Node));

	fseek(file, 0, SEEK_END);
	int length = ftell(file) / sizeof(int);
	rewind(file);

	int buffer[length];
	fread(buffer, sizeof(int), length, file);

	list->data = buffer[0];
	list->next = NULL;

	int i;
	for(i = 1; i < length; i++)
	{
		append_index(&list, -1, buffer[i]);
	}

	fclose(file);
	return list;
}

Node *create(int input_data)
{
	Node *list = malloc(sizeof(Node));
	list->data = input_data;
	list->next = NULL;
	return list;
}

void append_index(Node **ptrlist, int index, int input_data)
{
	Node *node = malloc(sizeof(Node));
	node->data = input_data;

	Node *cur_node = *ptrlist;
	if(index == 0) {
		node->next = cur_node;
		*ptrlist = node;
	} else if(index == -1) {
		node->next = NULL;

		while(cur_node->next)
		{
			cur_node = cur_node->next;
		}
		cur_node->next = node;
	} else {
		int i;
		for(i = 0; i < index - 1; i++)
		{
			cur_node = cur_node->next;
		}
		node->next = cur_node->next;
		cur_node->next = node;
	}
}

void delete_index(Node **ptrlist, int index)
{
	Node *cur_node = *ptrlist;
	if(index == 0) {
		*ptrlist = (*ptrlist)->next;
		free(cur_node);
	} else if(index == -1) {
		while(cur_node->next->next)
		{
			cur_node = cur_node->next;
		}
		free(cur_node->next);
		cur_node->next = NULL;
	} else {
		int i;
		for(i = 0; i < index - 1; i++)
		{
			cur_node = cur_node->next;
		}
		Node *ptrindex = cur_node->next;
		cur_node->next = cur_node->next->next;
		free(ptrindex);
	}
}

void print_list(Node *list)
{
	Node *cur_node = list;
	printf("%d ", cur_node->data);
	while(cur_node->next)
	{
		printf("%d ", cur_node->next->data);
		cur_node = cur_node->next;
	}
	printf("\n");
}

void print_help()
{
	printf("USAGE: linked_lists <file> <action> [action params]\n\
ACTION        PARAMS\n\
c      create value\n\
p      print\n\
d      delete index\n\
i      insert value index\n");
	exit(0);
}

int main(int argc, char *argv[])
{
	if (argc < 3) print_help();

	char *filename = argv[1];
	if(!strcmp("help", filename)) print_help();

	char action = argv[2][0];
	Node *list = NULL;
	switch(action) {
		case 'c':
			list = create(atoi(argv[3]));
			file_write(list, filename);
			break;
		case 'i':
			list = file_read(filename);
			append_index(&list, atoi(argv[4]), atoi(argv[3]));
			file_write(list, filename);
			break;
		case 'd':
			list = file_read(filename);
			delete_index(&list, atoi(argv[3]));
			file_write(list, filename);
			break;
		case 'p':
			list = file_read(filename);
			print_list(list);
			break;
		default:
			print_help();
	}

	return 0;
}
