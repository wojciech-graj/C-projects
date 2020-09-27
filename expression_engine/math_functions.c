#include "math_functions.h"

double add(double a, double b)
{
	return a + b;
}

double subtract(double a, double b)
{
	return a - b;
}

double multiply(double a, double b)
{
	return a * b;
}

double divide(double a, double b)
{
	return a / b;
}

double factorial(double a, double b)
{
	(void)b;
	double product = 1;
	for(;a > 1; a--)
	{
		product *= a;
	}
	return product;
}
