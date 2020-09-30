#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glu.h>

#include "../expression_engine/expression_engine.h"
#include <string.h>

int screen_size[2];
double x_min;
double x_max;

Node *head;

void draw_graph(void) {

	double dx = (x_max - x_min) / screen_size[0];

	glClearColor (1.0f, 1.0f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	substitute_variable(head, 'x', x_min-1);
	double prev_y = evaluate_tree(head);
	double x;
	glLineWidth(3.0f);
	glEnable(GL_LINE_SMOOTH);
	for(x = x_min; x <= x_max; x += dx)
	{
		substitute_variable(head, 'x', x);
		double y = evaluate_tree(head);
		glBegin(GL_LINES);

			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex2f(x - dx, prev_y);
			glColor3f(1.0f, 1.0f, 1.0f);
			glVertex2f(x, y);
		glEnd();
		prev_y = y;
	}

	glutSwapBuffers();
}

int main(int argc, char *argv[])
{
	if(argc != 8) {
		printf("ARGUMENTS: \"expression\" screen_size_x screen_size y x_min x_max y_min y_max\n");
		return 0;
	}

	char *input = argv[1];
	x_min = atof(argv[4]);
	x_max = atof(argv[5]);
	screen_size[0] = atoi(argv[2]);
	screen_size[1] = atoi(argv[3]);

	//expression_engine
	char (*tokens)[TOKEN_AMOUNT][TOKEN_LENGTH] = malloc(TOKEN_AMOUNT * TOKEN_LENGTH * sizeof(char));
	int tokens_amount = tokenize(tokens, input);
	convert_tokens_to_nodes(&head, tokens, tokens_amount);

	//GLUT
	int glut_argc = 1;
	char * glut_argv[1] = {" "};
	glutInit(&glut_argc, glut_argv);
	/*glutInitWindowPosition(-1, -1);*/
	glutInitWindowSize(screen_size[0], screen_size[1]);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE | GLUT_DEPTH);
	glutCreateWindow("Graph2d");
	gluOrtho2D(x_min, x_max, atoi(argv[6]), atoi(argv[7]));
	glutDisplayFunc(draw_graph);


	glutMainLoop();

	return 0;
}
