#include "texture.h"

void init_texture(unsigned char (*image)[4], short *dimensions, unsigned int texture)
{
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dimensions[X], dimensions[Y], 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
}

void load_textures(char *filename, Context *context)
{
	glGenTextures(NUM_TEXTURES, context->textures);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	FILE *file = fopen(filename, "rb");
	assert(file);

	int i;
	for(i = 0; i < NUM_TEXTURES; i++)
	{
		short image_dimensions[2];
		fread(image_dimensions, sizeof(short), 2, file);

		int image_size = image_dimensions[X] * image_dimensions[Y];
		unsigned char image[image_size][4];
		fread(image, sizeof(unsigned char), image_size * 4, file);

		init_texture(image, image_dimensions, i);
	}

	fclose(file);
}
