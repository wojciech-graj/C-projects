#ifndef TEXTURE_H
#define TEXTURE_H

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <GL/gl.h>

#include "context.h"
#include "global.h"

void init_texture(unsigned char (*image)[4], short *dimensions, unsigned int texture);
void load_textures(char *filename, Context *context);

#endif
