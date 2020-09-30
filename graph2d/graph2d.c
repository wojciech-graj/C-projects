#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glu.h>

#include "../expression_engine/expression_engine.h"
#include <string.h>

const int SCREEN_SIZE[] = {320, 320};
int x_min;
int x_max;

Node *head;

void draw_graph(void) {

	glClear(GL_COLOR_BUFFER_BIT);

	substitute_variable(head, 'x', x_min-1);
	double prev_y = evaluate_tree(head);
	int x;
	for(x = x_min; x <= x_max; x++)
	{
		substitute_variable(head, 'x', x);
		double y = evaluate_tree(head);
		glBegin(GL_LINES);
			glColor3f(1.0f, 1.0f, 1.0f);
			glVertex2f(x - 1, prev_y);
			glColor3f(1.0f, 1.0f, 1.0f);
			glVertex2f(x, y);
		glEnd();
		prev_y = y;
	}

	glutSwapBuffers();
}

int main(int argc, char *argv[])
{
	if(argc != 6) {
		printf("ARGUMENTS: \"expression\" x_min x_max y_min y_max\n");
		return 0;
	}

	char *input = argv[1];
	x_min = atoi(argv[2]);
	x_max = atoi(argv[3]);

	//expression_engine
	char (*tokens)[TOKEN_AMOUNT][TOKEN_LENGTH] = malloc(TOKEN_AMOUNT * TOKEN_LENGTH * sizeof(char));
	int tokens_amount = tokenize(tokens, input);
	convert_tokens_to_nodes(&head, tokens, tokens_amount);

	//GLUT
	int glut_argc = 1;
	char * glut_argv[1] = {" "};
	glutInit(&glut_argc, glut_argv);
	/*glutInitWindowPosition(-1, -1);*/
	glutInitWindowSize(SCREEN_SIZE[0], SCREEN_SIZE[1]);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE | GLUT_DEPTH);
	glutCreateWindow("Graph2d");
	gluOrtho2D(x_min, x_max, atoi(argv[4]), atoi(argv[5]));
	glutDisplayFunc(draw_graph);

	glutMainLoop();

	return 0;
}
