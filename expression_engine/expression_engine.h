#ifndef _EXPRESSION_ENGINE_H_
#define _EXPRESSION_ENGINE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

typedef struct Node Node;
typedef struct ListNode ListNode;

const char *L5OPS[] = {"+", "-"};
const char *L4OPS[] = {"/", "*", "%"};
const char *L3OPS[] = {"^"};
const char *L2OPS[] = {"!", "ln", "log", "sqrt", "sin", "cos", "tan", "sinh", "cosh", "tanh"};
const char *L1OPS[] = {"(", ")"};
const char *L0OPS[] = {"pi", "e", "phi"};
const int OPSLEN[] = {3, 2, 10, 1, 3, 2};
const double L0OPVALS[] = {M_PI, M_E, (1+sqrt(5))/2};
const char OPTYPES[] = {'c', '\0', '2', '3', '4', '5'};
const int NUMOPTYPES = 6;

const int TOKEN_LENGTH = 6;
const int TOKEN_AMOUNT = 30;

#endif
