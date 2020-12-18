#include "texture.h"

void init_texture(unsigned char (*image)[4], short *dimensions, unsigned int texture_id)
{
	glBindTexture(GL_TEXTURE_2D, texture_id);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dimensions[X], dimensions[Y], 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
}

void load_textures(char *filename, Context *context)
{
	FILE *file = fopen(filename, "rb");
	assert(file);
	assert(fread(&context->num_textures, sizeof(short), 1, file) == 1);
	context->textures = malloc(sizeof(unsigned int) * context->num_textures);
	glGenTextures(context->num_textures, context->textures);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	int i;
	for(i = 0; i < context->num_textures; i++)
	{
		short image_dimensions[2];
		assert(fread(image_dimensions, sizeof(short), 2, file) == 2);

		int image_size = image_dimensions[X] * image_dimensions[Y];
		unsigned char image[image_size][4];
		assert(fread(image, sizeof(unsigned char), image_size * 4, file) == (size_t)(image_size * 4));

		init_texture(image, image_dimensions, context->textures[i]);
	}

	fclose(file);
}
