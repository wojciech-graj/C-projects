//creates a binary file which contains level information obtained from the input file
//data types:
//	short: level width, level height, level topology, goal coordinates
//	unsigned char: floor colors, left color, right color
//output file contains(in order): level width, level height, level topology, floor colors, left color, right color, goal coordinates
//usage: ./level_serializer input.txt output

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdio.h>

void write_array_line(char *buffer, FILE *output_file, int num_elems, size_t size)
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
	bool level = false;
	bool color = false;
	bool area = false;
	int element_num = 0;
	while (fgets(buffer, 254, input_file))
    {
        buffer[strcspn(buffer, "\n")] = '\0';
		if(! strncmp("WIDTH", buffer, 5)) {
			assert(element_num++ == 0);
			memmove(buffer, buffer+6, strlen(buffer));
			level_size[0] = atoi(buffer);
		} else if(! strncmp("HEIGHT", buffer, 6)) {
			assert(element_num++ == 1);
			memmove(buffer, buffer+7, strlen(buffer));
			level_size[1] = atoi(buffer);
		} else if(! strncmp("START", buffer, 5)) {
			memmove(buffer, buffer+5, strlen(buffer));
			if(! strncmp("LEVEL", buffer, 5)) {
				assert(element_num++ == 2);
				fwrite(level_size, sizeof(short), 2, output_file);
				level = true;
			} else if(! strncmp("COLOR", buffer, 5)) {
				assert(element_num++ == 4);
				color = true;
			} else if(! strncmp("AREA", buffer, 4)) {
				assert(element_num++ == 8);
				area = true;
			}
		} else if(! strncmp("END", buffer, 3)) {
			memmove(buffer, buffer+3, strlen(buffer));
			if(! strncmp("LEVEL", buffer, 5)) {
				assert(num_rows == level_size[1] && element_num++ == 3);
				level = false;
			} else if(! strncmp("COLOR", buffer, 5)) {
				assert(num_rows == level_size[1] && element_num++ == 5);
				color = false;
			} else if(! strncmp("AREA", buffer, 4)) {
				assert(num_rows == 5 && element_num++ == 9);
				area = false;
			}
			num_rows = 0;
		} else if(level) {
			num_rows++;
			write_array_line(buffer, output_file, level_size[0] * 5, sizeof(short));
		} else if(color) {
			num_rows++;
			write_array_line(buffer, output_file, level_size[0] * 3, sizeof(unsigned char));
		} else if(! strncmp("COLOR", buffer, 5)) {
			memmove(buffer, buffer+5, strlen(buffer));
			if(! strncmp("LEFT", buffer, 4)) {
				assert(element_num++ == 6);
			} else if(! strncmp("RIGHT", buffer, 5)) {
				assert(element_num++ == 7);
			} else {
				printf("UNRECOGNIZED COMMAND: %s\n", buffer);
				return 1;
			}
			write_array_line(buffer, output_file, 3, sizeof(unsigned char));
		} else if(area) {
			if(! strncmp("TEXTURE", buffer, 7)) {
				assert(num_rows++ == 0);
				write_array_line(buffer, output_file, 1, sizeof(int));
			} else if(! strncmp("COORDINATES", buffer, 11)) {
				assert(num_rows++ == 1);
				write_array_line(buffer, output_file, 8, sizeof(short));
			} else if(! strncmp("ROTATE", buffer, 6)) {
				assert(num_rows++ == 2);
				write_bool(buffer[7], output_file);
			} else if(! strncmp("FLIPX", buffer, 5)) {
				assert(num_rows++ == 3);
				write_bool(buffer[6], output_file);
			} else if(! strncmp("FLIPY", buffer, 5)) {
				assert(num_rows++ == 4);
				write_bool(buffer[6], output_file);
			} else {
				printf("UNRECOGNIZED COMMAND: %s\n", buffer);
				return 1;
			}
		}
    }

    fclose(input_file);
	fclose(output_file);
    return 0;
}
