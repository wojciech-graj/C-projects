#ifndef _GRAPH3D_H_
#define _GRAPH3D_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glu.h>

#include "../expression_engine/expression_engine.h"
#include "../expression_engine/math_functions.h"

typedef struct AdaptiveColor AdaptiveColor;
typedef struct ConfigData ConfigData;

const int BUFFER_SIZE = 255;
const float WHITE[3] = {1.0, 1.0, 1.0};
const float RED[3] = {1.0, 0.0, 0.0};
const float GREEN[3] = {0.0, 1.0, 0.0};
const float BLUE[3] = {0.0, 0.0, 1.0};

const int FPS = 30;
const double FRAME_DELAY = 1000 / FPS;

#endif
