#ifndef TEXTURE_H
#define TEXTURE_H

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

#include <GL/gl.h>

#include "global_constants.h"
#include "context.h"

void init_texture(unsigned char (*image)[4], short *dimensions, unsigned int texture);
void load_textures(char *filename, Context *context);

#endif
