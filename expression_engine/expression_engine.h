#ifndef _EXPRESSION_ENGINE_H_
#define _EXPRESSION_ENGINE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

typedef struct Node Node;

const char L1BINOPS[] = {'+', '-'};
const char L2BINOPS[] = {'/', '*'};
const char L3BINOPS[] = {'^'};
const int OPSLEN[] = {2, 2, 1};

const char ORDEROPS[] = {'n', '3', '2', '1'};
const int ORDEROPSLEN = 4;

const int TOKEN_LENGTH = 5;
const int TOKEN_AMOUNT = 10;

#endif
