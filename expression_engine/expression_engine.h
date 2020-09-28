#ifndef _EXPRESSION_ENGINE_H_
#define _EXPRESSION_ENGINE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

typedef struct Node Node;

const char *L5OPS[] = {"+", "-"};
const char *L4OPS[] = {"/", "*"};
const char *L3OPS[] = {"^", "sqrt"};
const char *L2OPS[] = {"!"};
//math constants
const char *L0OPS[] = {"pi", "e", "phi"};
const double L0OPVALS[] = {M_PI, M_E, (1+sqrt(5))/2};

const int OPSLEN[] = {3, 0, 1, 2, 2, 2};



const int NUMLEVELS = 5;

const int TOKEN_LENGTH = 5;
const int TOKEN_AMOUNT = 10;

#endif
