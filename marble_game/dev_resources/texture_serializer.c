//creates a binary file which contains data obtained from images
//each line of the input file contains the filename of a .bmp image
//the images are read in order, and must have 32 bits, with 8 bits for each of A, R, G, B
//the blank output file is created, then image width, image height, pixel colors are appended to the file for each image
//data types:
//	short: image width, image height
//	unsigned char: pixel color [4] : {R, G, B, A}
//output file contains(in order): image width, image height, pixel colors
//usage: ./texture_serializer input.txt output

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

const int bmp_header_size = 54;
typedef struct BmpHeader {
    unsigned char ID[2];
    unsigned int filesize;
    unsigned int RESERVED;
    unsigned int bitmap_data_offset;
    unsigned int bitmap_header_size;
    unsigned int width;
    unsigned int height;
    unsigned short nr_of_planes;
    unsigned short bits_per_pixel;
    unsigned int compression;
    unsigned int bitmap_data_size;
    unsigned int horizontal_resolution;
    unsigned int vertical_resolution;
    unsigned int colors;
    unsigned int important_colors;
} BmpHeader;

void read_bmp_header(BmpHeader **bmp_header, FILE *file)
{
	(*bmp_header) = malloc(sizeof(BmpHeader));
	fread(&((*bmp_header)->ID), sizeof(char), 2, file);
	fread(&((*bmp_header)->filesize), bmp_header_size - sizeof((*bmp_header)->ID), 1, file);
}

int main(int argc, char *argv[])
{
	assert(argc == 3);
	FILE *input_file = fopen(argv[1], "r");
	assert(input_file);
	FILE *output_file = fopen(argv[2], "w+b");
	assert(output_file);

	char buffer[255];
	while (fgets(buffer, 254, input_file))
    {
		buffer[strcspn(buffer, "\n")] = '\0';
		FILE *bmp_file = fopen(buffer, "r");
		assert(bmp_file);

		BmpHeader *bmp_header = NULL;
		read_bmp_header(&bmp_header, bmp_file);
		assert(bmp_header);
		assert(bmp_header->bits_per_pixel == 32);

		short image_dimensions[2] = {bmp_header->width, bmp_header->height};
		fwrite(image_dimensions, sizeof(short), 2, output_file);

		fseek(bmp_file, bmp_header->bitmap_data_offset, 0);
		int i;
		for(i = 0; i < image_dimensions[0] * image_dimensions[1]; i++)
		{
			unsigned char pixel_argb[4];
			fread(pixel_argb, sizeof(unsigned char), 4, bmp_file);
			unsigned char pixel_rgba[4] = {pixel_argb[1], pixel_argb[2], pixel_argb[3], pixel_argb[0]};
			fwrite(pixel_rgba, sizeof(unsigned char), 4, output_file);
		}

		fclose(bmp_file);
		free(bmp_header);
	}

	fclose(input_file);
	fclose(output_file);
    return 0;
}
