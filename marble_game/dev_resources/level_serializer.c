//creates a binary file which contains level information obtained from the input file.
//all output values are of type short
//output file contains(in order): level width, level height, level data
//usage: ./level_serializer input.txt output

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdio.h>

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
	while (fgets(buffer, 254, input_file))
    {
        buffer[strcspn(buffer, "\n")] = '\0';
		if(! strncmp("WIDTH", buffer, 5)) {
			memmove(buffer, buffer+6, strlen(buffer));
			level_size[0] = atoi(buffer);
		} else if(! strncmp("HEIGHT", buffer, 6)) {
			assert(level_size[0]);
			memmove(buffer, buffer+7, strlen(buffer));
			level_size[1] = atoi(buffer);
		} else if(! strncmp("LEVELSTART", buffer, 10)) {
			assert(level_size[0] && level_size[1]);
			fwrite(level_size, sizeof(short), 2, output_file);
			level = true;
		} else if(! strncmp("LEVELEND", buffer, 8)) {
			assert(num_rows == level_size[1]);
			level = false;
		} else if(level) {
			num_rows++;
			short cur_num = 0;
			short num_cols = 0;
			int i;
			for(i = 0; buffer[i] != '\0'; i++)
			{
				if(isdigit(buffer[i])) {
					cur_num = 10 * cur_num + buffer[i] - 48;
				} else if(cur_num != 0) {
					fwrite(&cur_num, sizeof(short), 1, output_file);
					cur_num = 0;
					num_cols++;
				}
			}
			assert(num_cols == level_size[0] * 4);
		}
    }

    fclose(input_file);
	fclose(output_file);
    return 0;
}
