#ifndef MATH_FUNCTIONS_H
#define MATH_FUNCTIONS_H

#include <math.h>

#include "global.h"

//calculates the z value of point (posx,posy) on the plane which intersects (x1,y1,z1),(x2,y2,z2),(x3,y3,z3)
static inline float calculate_z_on_plane(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, float posx, float posy) {
	float v1[3] = {x2 - x1, y2 - y1, z2 - z1};
	float v2[3] = {x3 - x2, y3 - y2, z3 - z2};
	float v1xv2[3] = {v1[Y]*v2[Z] - v1[Z]*v2[Y],
		v1[Z]*v2[X] - v1[X]*v2[Z],
		v1[X]*v2[Y] - v1[Y]*v2[X]}; //cross product of v1 and v2
	float d = v1xv2[X] * x1 + v1xv2[Y] * y1 + v1xv2[Z] * z1;
	return (d - v1xv2[X] * posx - v1xv2[Y] * posy) / v1xv2[Z];
}

static inline float dot_product(float *v1, float *v2)
{
	return v1[X] * v2[X] + v1[Y] * v2[Y];
}

static inline float distance(float *p1, float *p2)
{
	return sqrt(pow(p2[X] - p1[X], 2) + pow(p2[Y] - p1[Y], 2));
}

static inline int mini(int a, int b)
{
	return ((a < b) ? a : b);
}

static inline int maxi(int a, int b)
{
	return ((a > b) ? a : b);
}

static inline int sqri(int a)
{
	return (a * a);
}

static inline bool equalf(float a, float b, float epsilon)
{
	return (float) fabs(a - b) < epsilon;
}

//euclidean modulo
static inline int eucmod(const int a, const int b)
{
    return (a < 0 ? (((a % b) + b) % b) : (a % b));
}

#endif
