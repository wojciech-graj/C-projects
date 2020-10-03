#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glu.h>

#include "../expression_engine/expression_engine.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct ConfigFunction ConfigFunction;

const int BUFFER_SIZE = 255;
const float BLACK[3] = {0.0, 0.0, 0.0};
const float RED[3] = {1.0, 0.0, 0.0};
const float GREEN[3] = {0.0, 1.0, 0.0};
const float BLUE[3] = {0.0, 0.0, 1.0};

typedef struct ConfigFunction {
	Node *head;
	const float *color;
	ConfigFunction *func_next;
} ConfigFunction;

typedef struct ConfigData {
	int screen_size_x;
	int screen_size_y;
	double min_x;
	double min_y;
	double max_x;
	double max_y;
	double dx;
	double dy;
	double scale_x;
	double scale_y;
	int digits_x;
	int digits_y;
	int axis_offset;
	ConfigFunction *func_head;
} ConfigData;

ConfigData *config;

void RenderFloat(float x, float y, void *font, double num, int digits)
{
	char string[BUFFER_SIZE];
	gcvt(num,digits,string);
	glRasterPos2f(x, y);
    glutBitmapString(font, string);
}

void draw_graph(void) {
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	double x;
	glLineWidth(3.0f);
	ConfigFunction *cur_function = config->func_head;

	while(cur_function)
	{
		glBegin(GL_LINE_STRIP);
			glColor3f(cur_function->color[0], cur_function->color[1], cur_function->color[2]);
			for(x = config->min_x - config->dx; x <= config->max_x; x += config->dx)
			{
				substitute_variable(cur_function->head, 'x', x);
				double y = evaluate_tree(cur_function->head);
				glVertex2f(x, y);
			}
		glEnd();
		cur_function = cur_function->func_next;
	}

	double i;
	glBegin(GL_LINES);
		glColor3f(0.0f, 0.0f, 0.0f);
		//x-axis
		glVertex2f(config->min_x, config->min_y);
		glVertex2f(config->max_x, config->min_y);
		//y-axis
		glVertex2f(config->min_x, config->min_y);
		glVertex2f(config->min_x, config->max_y);

		//x-axis ticks
		for(i = config->min_x; i < config->max_x; i += config->scale_x)
		{
			glVertex2f(i, config->min_y);
			glVertex2f(i, config->min_y + config->dy * 12);
		}
		//y-axis ticks
		for(i = config->min_y; i < config->max_y; i += config->scale_y)
		{
			glVertex2f(config->min_x, i);
			glVertex2f(config->min_x + config->dx * 12, i);
		}
	glEnd();

	//x-axis labels
	for(i = config->min_x; i < config->max_x; i += config->scale_x)
	{
		RenderFloat(i - 6 * config->dx, -24 * config->dy, GLUT_BITMAP_TIMES_ROMAN_24, i, config->digits_x);
	}
	//y-axis labels
	for(i = config->min_y; i < config->max_y; i += config->scale_y)
	{
		RenderFloat(config->dx * -1 * config->axis_offset, i - 6 * config->dy, GLUT_BITMAP_TIMES_ROMAN_24, i, config->digits_y);
	}

	glutSwapBuffers();
}

//TODO:REWORK
void read_config(char *filename)
{
	FILE *f = fopen(filename, "r");
	char *buf = malloc(BUFFER_SIZE);
	config = malloc(sizeof(ConfigData));
	ConfigFunction *cur_function = NULL;


	while(fgets(buf, BUFFER_SIZE, f)) {
		(void) strtok(buf, "\n");
		if(buf[0] == 'F') {
			memmove(buf, buf+2, strlen(buf));

			Node *head;
			char (*tokens)[TOKEN_AMOUNT][TOKEN_LENGTH] = malloc(TOKEN_AMOUNT * TOKEN_LENGTH * sizeof(char));
			int tokens_amount = tokenize(tokens, buf);
			convert_tokens_to_nodes(&head, tokens, tokens_amount);

			ConfigFunction *function = malloc(sizeof(ConfigFunction));
			function->head = head;
			function->func_next = NULL;
			function->color = BLACK;
			if(! cur_function) {
				config->func_head = function;
			} else {
				cur_function->func_next = function;
			}
			cur_function = function;
		} else if(buf[0] == 'X') {
			if(buf[1] == 'r') {
				memmove(buf, buf+5, strlen(buf));
				config->screen_size_x = atoi(buf);
			} else if(buf[3] == 'x') {
				memmove(buf, buf+5, strlen(buf));
				config->max_x = atof(buf);
			} else if(buf[3] == 'n') {
				memmove(buf, buf+5, strlen(buf));
				config->min_x = atof(buf);
			} else if(buf[1] == 's') {
				memmove(buf, buf+5, strlen(buf));
				config->scale_x = atof(buf);
			} else if(buf[1] == 'd') {
				memmove(buf, buf+5, strlen(buf));
				config->digits_x = atof(buf);
			}
		} else if(buf[0] == 'Y') {
			if(buf[1] == 'r') {
				memmove(buf, buf+5, strlen(buf));
				config->screen_size_y = atoi(buf);
			} else if(buf[3] == 'x') {
				memmove(buf, buf+5, strlen(buf));
				config->max_y = atof(buf);
			} else if(buf[3] == 'n') {
				memmove(buf, buf+5, strlen(buf));
				config->min_y = atof(buf);
			} else if(buf[1] == 's') {
				memmove(buf, buf+5, strlen(buf));
				config->scale_y = atof(buf);
			} else if(buf[1] == 'd') {
				memmove(buf, buf+5, strlen(buf));
				config->digits_y = atoi(buf);
			}
		} else if(buf[0] == 'C') {
			memmove(buf, buf+2, strlen(buf));
			if(! strcmp(buf, "RED")) {
				cur_function->color = RED;
			} else if(! strcmp(buf, "GREEN")) {
				cur_function->color = GREEN;
			} else if(! strcmp(buf, "BLUE")) {
				cur_function->color = BLUE;
			}
		}
	}
	config->dx = (config->max_x - config->min_x) / config->screen_size_x;
	config->dy = (config->max_y - config->min_y) / config->screen_size_y;
	config->axis_offset = 14 * config->digits_y + 18;
	fclose(f);
	free(buf);
}

int main(int argc, char *argv[])
{
	read_config("config");

	//GLUT
	int glut_argc = 1;
	char * glut_argv[1] = {" "};
	glutInit(&glut_argc, glut_argv);
	glutInitWindowSize(config->screen_size_x, config->screen_size_y);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE | GLUT_DEPTH);
	glutCreateWindow("Graph2d");
	gluOrtho2D(config->min_x - config->dx * config->axis_offset, config->max_x, config->min_y - config->dy * config->axis_offset, config->max_y);
	glutDisplayFunc(draw_graph);

	glutMainLoop();

	free(config);
	return 0;
}
