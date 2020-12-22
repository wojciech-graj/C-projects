#ifndef TEXTURE_P_H
#define TEXTURE_P_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <GL/gl.h>

#include "inc/context.h"

void init_texture(unsigned char (*image)[4], short *dimensions, unsigned int texture);

#endif
