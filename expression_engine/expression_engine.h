#ifndef _EXPRESSION_ENGINE_H_
#define _EXPRESSION_ENGINE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

typedef struct Node Node;

//binary
const char L5OPS[] = {'+', '-'};
const char L4OPS[] = {'/', '*'};
const char L3OPS[] = {'^'};
//unary
const char L2OPS[] = {'!'};

const int OPSLEN[] = {0, 0, 1, 1, 2, 2};

const char *MATHCONSTS[] = {"pi", "e", "phi"};
const double MATHCONSTVALS[] = {M_PI, M_E, (1+sqrt(5))/2};
const int MATHCONSTSLEN = 3;

const int NUMLEVELS = 5;

const int TOKEN_LENGTH = 5;
const int TOKEN_AMOUNT = 10;

#endif
