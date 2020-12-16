//TODO: CREATE NEW DOCSTRING

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdio.h>

#include "../src/global.h"

#define LEVEL -1
#define COLOR -2
#define NO_ITEM -3

enum first_elements{WIDTH, HEIGHT, STARTLEVEL, ENDLEVEL, STARTCOLOR, ENDCOLOR, COLORLEFT, COLORRIGHT, NUMOBJECTS};

//TODO: REWORK FUNCTION
void write_int_array_line(char *buffer, FILE *output_file, int num_elems, size_t size)
{
	int cur_num = 0;
	int sign = 1;
	int num_cols = 0;
	int i;
	for(i = 0; buffer[i] != '\0'; i++)
	{
		if(buffer[i] == '-') {
			sign = -1;
		}else if(isdigit(buffer[i])) {
			if(buffer[i] == '0' && cur_num == 0) goto WRITE;
			cur_num = 10 * cur_num + buffer[i] - '0';
		} else if(cur_num != 0) {
			WRITE:
			fwrite(&cur_num, size, 1, output_file);
			cur_num = 0;
			sign = 1;
			num_cols++;
		}
	}
	assert(num_cols == num_elems);
}

void write_float_array_line(char *buffer, FILE *output_file, int num_elems)
{
	float cur_num = 0;
	int sign = 1;
	bool decimal = false;
	int decimal_divisor = 10;
	int num_cols = 0;
	int i;
	for(i = 0; buffer[i] != '\0'; i++)
	{
		if(buffer[i] == '-') {
			sign = -1;
		} else if(buffer[i] == '.') {
			decimal = true;
		} else if(isdigit(buffer[i])) {
			if(buffer[i] == '0' && ! isdigit(buffer[i + 1])) goto WRITE;
			if(decimal) {
				cur_num += (float) (buffer[i] - '0') / decimal_divisor;
				decimal_divisor *= 10;
			} else {
				cur_num = 10 * cur_num + buffer[i] - '0';
			}
		} else if(cur_num != 0) {
			WRITE:
			fwrite(&cur_num, sizeof(float), 1, output_file);
			cur_num = 0;
			sign = 1;
			decimal = false;
			decimal_divisor = 10;
			num_cols++;
		}
	}
	assert(num_cols == num_elems);
}

void write_bool(char text, FILE *output_file)
{
	bool selection;
	switch(text)
	{
		case 'F':
			selection = false;
			break;
		case 'T':
			selection = true;
			break;
		default:
			printf("INVALID BOOLEAN VALUE\n");
			exit(1);
	}
	fwrite(&selection, sizeof(bool), 1, output_file);
}

int get_texture_index(char *name)
{
	if(! strncmp("T_GOAL", name, 6)) {
		return T_GOAL;
	} else if(! strncmp("T_FLAG_RED", name, 6)) {
		return T_FLAG_RED;
	} else {
		printf("UNRECOGNIZED TEXTURE: %s\n", name);
		exit(1);
	}
}

void write_int(char *buffer, int offset, FILE *output_file)
{
	memmove(buffer, buffer+offset, strlen(buffer));
	int var = atoi(buffer);
	fwrite(&var, sizeof(int), 1, output_file);
}

void write_float(char *buffer, int offset, FILE *output_file)
{
	memmove(buffer, buffer+offset, strlen(buffer));
	float var = atof(buffer);
	fwrite(&var, sizeof(float), 1, output_file);
}

void write_tile_positions_as_corner_positions(char *buffer, FILE *output_file, float ymul)
{
	int cur_num = 0;
	int cur_index = 0;
	int i;
	int tile_positions[8];
	for(i = 0; buffer[i] != '\0'; i++)
	{
		if(isdigit(buffer[i])) {
			if(buffer[i] == '0' && cur_num == 0) goto APPENDPOSITION;
			cur_num = 10 * cur_num + buffer[i] - '0';
		} else if(cur_num != 0) {
			APPENDPOSITION:
			tile_positions[cur_index] = cur_num;
			cur_num = 0;
			cur_index++;
		}
	}
	assert(cur_index == 8);

	float positions[8];
	for(i = 0; i < 8; i += 2)
	{
		int offset = tile_positions[i + 1] % 2;
		positions[i] = tile_positions[i] + offset/2.f + ((i/2 - 1) % 2)/2.f;
		positions[i + 1] = tile_positions[i + 1]/2.f + ((i/2 - 2) % 2)/2.f;
		positions[i + 1] *= ymul;
	}
	fwrite(positions, sizeof(float), 8, output_file);
}

//TODO FUNCTION: INDEX IN STRING ARRAY

int index_in_string_array(char *string, char *array[], int array_length)
{
	int string_length = strlen(string);
	int i;
	for(i = 0; i < array_length; i++)
	{
		if(! strncmp(array[i], string, string_length)) {
			return i;
		}
	}
}

int main(int argc, char *argv[])
{
	assert(argc == 3);
	FILE *input_file = fopen(argv[1], "r");
	assert(input_file);
	FILE *output_file = fopen(argv[2], "w+b");
	assert(output_file);
	char buffer[255];
	short level_size[2] = {0, 0}; //width, height
	short num_rows = 0;
	int cur_item = NO_ITEM;
	int element_num = 0;

	short object_num_rows[5];
	object_num_rows[MARBLE] = 1;
	object_num_rows[AREA] = 3;
	object_num_rows[POINT] = 3;
	object_num_rows[SPRITE] = 6;
	object_num_rows[COLLISIONAREA] = 2;

	while (fgets(buffer, 254, input_file))
    {
		//printf("%s", buffer);
        buffer[strcspn(buffer, "\n")] = '\0';
		switch(cur_item)
		{
			case NO_ITEM:
			if(! strncmp("WIDTH", buffer, 5)) {
				assert(element_num++ == WIDTH);
				memmove(buffer, buffer+6, strlen(buffer));
				level_size[0] = atoi(buffer);
			} else if(! strncmp("HEIGHT", buffer, 6)) {
				assert(element_num++ == HEIGHT);
				memmove(buffer, buffer+7, strlen(buffer));
				level_size[1] = atoi(buffer);
			} else if(! strncmp("NUMOBJECTS", buffer, 10)) {
				assert(element_num++ == NUMOBJECTS);
				memmove(buffer, buffer+11, strlen(buffer));
				int num_objects = atoi(buffer);
				fwrite(&num_objects, sizeof(int), 1, output_file);
			} else if(! strncmp("START", buffer, 5)) {
				assert(cur_item == NO_ITEM);
				memmove(buffer, buffer+5, strlen(buffer));
				if(! strncmp("LEVEL", buffer, 5)) {
					assert(element_num++ == STARTLEVEL);
					fwrite(level_size, sizeof(short), 2, output_file);
					cur_item = LEVEL;
				} else if(! strncmp("COLOR", buffer, 5)) {
					assert(element_num++ == STARTCOLOR);
					cur_item = COLOR;
				} else if(! strncmp("AREA", buffer, 4)) {
					int area = AREA;
					fwrite(&area, sizeof(int), 1, output_file);
					cur_item = AREA;
				} else if(! strncmp("MARBLE", buffer, 6)) {
					int marble = MARBLE;
					fwrite(&marble, sizeof(int), 1, output_file);
					cur_item = MARBLE;
				} else if(! strncmp("COLLISIONAREA", buffer, 13)) {
					int collision_area = COLLISIONAREA;
					fwrite(&collision_area, sizeof(int), 1, output_file);
					cur_item = COLLISIONAREA;
				} else if(! strncmp("SPRITE", buffer, 6)) {
					int sprite = SPRITE;
					fwrite(&sprite, sizeof(int), 1, output_file);
					cur_item = SPRITE;
				} else if(! strncmp("POINT", buffer, 5)) {
					int point = POINT;
					fwrite(&point, sizeof(int), 1, output_file);
					cur_item = POINT;
				}
			} else if(! strncmp("COLOR", buffer, 5)) {
				memmove(buffer, buffer+5, strlen(buffer));
				if(! strncmp("LEFT", buffer, 4)) {
					assert(element_num++ == COLORLEFT);
				} else if(! strncmp("RIGHT", buffer, 5)) {
					assert(element_num++ == COLORRIGHT);
				} else {
					printf("UNRECOGNIZED COMMAND: %s\n", buffer);
					return 1;
				}
				write_int_array_line(buffer, output_file, 3, sizeof(unsigned char));
			}
			break;
			case LEVEL:
			if(! strncmp("ENDLEVEL", buffer, 8)) {
				assert(num_rows == level_size[1] && element_num++ == ENDLEVEL && cur_item == LEVEL);
				cur_item = NO_ITEM;
				num_rows = 0;
				break;
			}
			num_rows++;
			write_int_array_line(buffer, output_file, level_size[0] * 5, sizeof(short));
			break;
			case COLOR:
			if(! strncmp("ENDCOLOR", buffer, 8)) {
				assert(num_rows == level_size[1] && element_num++ == ENDCOLOR && cur_item == COLOR);
				cur_item = NO_ITEM;
				num_rows = 0;
				break;
			}
			num_rows++;
			write_int_array_line(buffer, output_file, level_size[0] * 3, sizeof(unsigned char));
			break;
			case MARBLE:
			if(! strncmp("COLOR", buffer, 5)) {
				assert(num_rows++ == 0);
				write_int_array_line(buffer, output_file, 3, sizeof(unsigned char));
			} else if(! strncmp("ENDMARBLE", buffer, 9)) {
				assert(num_rows == object_num_rows[MARBLE] && cur_item == MARBLE);
				cur_item = NO_ITEM;
				num_rows = 0;
			}
			break;
			case SPRITE:
			if(! strncmp("TEXTURE", buffer, 7)) {
				assert(num_rows++ == 0);
				memmove(buffer, buffer+8, strlen(buffer));
				int texture_index = get_texture_index(buffer);
				fwrite(&texture_index, sizeof(int), 1, output_file);
			} else if(! strncmp("FRAMETIME", buffer, 9)) {
				assert(num_rows++ == 1);
				write_int(buffer, 10, output_file);
			} else if(! strncmp("ROTATE", buffer, 6)) {
				assert(num_rows++ == 2);
				write_bool(buffer[7], output_file);
			} else if(! strncmp("FLIPX", buffer, 5)) {
				assert(num_rows++ == 3);
				write_bool(buffer[6], output_file);
			} else if(! strncmp("FLIPY", buffer, 5)) {
				assert(num_rows++ == 4);
				write_bool(buffer[6], output_file);
			} else if(! strncmp("TILEPOSITIONS", buffer, 13)) {
				assert(num_rows++ == 5);
				write_tile_positions_as_corner_positions(buffer, output_file, .5f);
			} else if(! strncmp("POSITIONS", buffer, 9)) {
				assert(num_rows++ == 5);
				write_float_array_line(buffer, output_file, 8);
			} else if(! strncmp("ENDSPRITE", buffer, 9)) {
				assert(num_rows == object_num_rows[SPRITE] && cur_item == SPRITE);
				cur_item = NO_ITEM;
				num_rows = 0;
			}
			break;
			case COLLISIONAREA:
			if(! strncmp("CANMOVEOVER", buffer, 11)) {
				assert(num_rows++ == 0);
				write_bool(buffer[12], output_file);
			} else if(! strncmp("TILEPOSITIONS", buffer, 13)) {
				assert(num_rows++ == 1);
				write_tile_positions_as_corner_positions(buffer, output_file, 1.f);
			} else if(! strncmp("ENDCOLLISIONAREA", buffer, 16)) {
				assert(num_rows == object_num_rows[COLLISIONAREA] && cur_item == COLLISIONAREA);
				cur_item = NO_ITEM;
				num_rows = 0;
			}
			break;
			case AREA:
			if(! strncmp("TILEPOSITIONS", buffer, 11)) {
				assert(num_rows++ == 0);
				write_int_array_line(buffer, output_file, 8, sizeof(short));
			} else if(! strncmp("SPRITE", buffer, 6)) {
				assert(num_rows++ == 1);
				write_int(buffer, 7, output_file);
			} else if(! strncmp("COLLISIONAREA", buffer, 13)) {
				assert(num_rows++ == 2);
				write_int(buffer, 14, output_file);
			} else if(! strncmp("ENDAREA", buffer, 7)) {
				assert(num_rows == object_num_rows[AREA] && cur_item == AREA);
				cur_item = NO_ITEM;
				num_rows = 0;
			}
			break;
			case POINT:
			if(! strncmp("SPRITE", buffer, 6)) {
				assert(num_rows++ == 0);
				write_int(buffer, 7, output_file);
			} else if(! strncmp("TILEPOSITION", buffer, 12)) {
				assert(num_rows++ == 1);
				write_int_array_line(buffer, output_file, 2, sizeof(short));
			} else if(buffer[0] == 'Y') {
				assert(num_rows++ == 2);
				write_float(buffer, 2, output_file);
			}else if(! strncmp("ENDPOINT", buffer, 8)) {
				assert(num_rows == object_num_rows[POINT] && cur_item == POINT);
				cur_item = NO_ITEM;
				num_rows = 0;
			}
			break;
		}
    }

    fclose(input_file);
	fclose(output_file);
    return 0;
}
