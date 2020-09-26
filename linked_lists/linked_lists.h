#ifndef LINKED_LISTS_H
#define LINKED_LISTS_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

typedef struct Node Node;
void append_index(Node **ptrlist, int index, int input_data);
void file_write(Node *list, char filename[]);
Node *file_read(char filename[]);
Node *create(int input_data);
void append_index(Node **ptrlist, int index, int input_data);
void delete_index(Node **ptrlist, int index);
void print_list(Node *list);

#endif
