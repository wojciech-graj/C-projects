#ifndef _GRAPH2D_H_
#define _GRAPH2D_H_

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glu.h>

#include "../expression_engine/expression_engine.h"

typedef struct ConfigFunction ConfigFunction;
typedef struct ConfigData ConfigData;

const int BUFFER_SIZE = 255;
const float BLACK[3] = {0.0, 0.0, 0.0};
const float RED[3] = {1.0, 0.0, 0.0};
const float GREEN[3] = {0.0, 1.0, 0.0};
const float BLUE[3] = {0.0, 0.0, 1.0};

#endif
