#ifndef MATH_FUNCTIONS_H
#define MATH_FUNCTIONS_H

#include "global.h"

float calculate_z_on_plane(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, float posx, float posy);
float dot_product(float *v1, float *v2);

static inline int imin(int a, int b) {
	return ((a < b) ? a : b);
}

static inline int imax(int a, int b) {
	return ((a > b) ? a : b);
}

#endif
