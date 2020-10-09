#include "graph3d.h"

const int FPS = 30;
const double FRAME_DELAY = 1000 / FPS;

bool redraw = true;

typedef struct ConfigData {
	int screen_size_x;
	int screen_size_y;
	double min_x;
	double min_y;
	double max_x;
	double max_y;
	double dx;
	double dy;
	int num_x;
	int num_y;
	double *z_values;
} ConfigData;

ConfigData *config;

float ang_hor = 0.0;
float ang_ver = 0.0;
float rad = 10;
float offset[3] = {0.0, 0.0, 0.0};

void draw_graph(void) {
	redraw = false;
	printf("draw\n");
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	gluLookAt(rad * sin(ang_ver) * cos(ang_hor) + offset[0],
			rad * sin(ang_hor) * sin(ang_ver) + offset[1],
			rad * cos(ang_ver) + offset[2],
			offset[0], offset[1],  offset[2],
			0.0f, 0.0f,  1.0f);

	double x;
	double y;
	int loc_x;
	int loc_y;
	glColor3f(0.0f, 1.0f, 0.0f);
	
	loc_x = 0;
	for(x = config->min_x; x <= config->max_x; x+=config->dx)
	{
		loc_y = 0;
		glBegin(GL_LINE_STRIP);
			for(y = config->min_y; y <= config->max_y; y+=config->dy) {
				glVertex3f(x, y, (config->z_values)[config->num_x*loc_x+loc_y]);
				loc_y += 1;
			}
		glEnd();
		loc_x += 1;
	}

	loc_y = 0;
	for(y = config->min_y; y <= config->max_y; y+=config->dy)
	{
		loc_x = 0;
		glBegin(GL_LINE_STRIP);
			for(x = config->min_x; x <= config->max_x; x+=config->dx) {
				glVertex3f(x, y, (config->z_values)[config->num_x*loc_x+loc_y]);
				loc_x += 1;
			}
		glEnd();
		loc_y += 1;
	}

	glutSwapBuffers();
}

Node *parse_config(char *filename)
{
	FILE *f = fopen(filename, "r");
	char buf[BUFFER_SIZE];
	config = malloc(sizeof(ConfigData));
	Node *head;

	while(fgets(buf, BUFFER_SIZE, f)) {
		if(strcmp(buf, "\n")) { //if not empty line
			strtok(buf, "\n");
			int hash = buf[0];
			int i;
			for(i = 1; i < 4; i++)
			{
				hash <<= i;
				hash ^= buf[i];

			}
			switch(hash)
			{

				case 6939: //Xres
					memmove(buf, buf+5, strlen(buf));
					config->screen_size_x = atoi(buf);
					break;
				case 7003: //Yres
					memmove(buf, buf+5, strlen(buf));
					config->screen_size_y = atoi(buf);
					break;
				case 6278: //Xmin
					memmove(buf, buf+5, strlen(buf));
					config->min_x = atof(buf);
					break;
				case 6352: //Xmax
					memmove(buf, buf+5, strlen(buf));
					config->max_x = atof(buf);
					break;
				case 6342: //Ymin
					memmove(buf, buf+5, strlen(buf));
					config->min_y = atof(buf);
					break;
				case 6288: //Ymax
					memmove(buf, buf+5, strlen(buf));
					config->max_y= atof(buf);
					break;
				default:
					if(buf[0] == 'F') {
						memmove(buf, buf+2, strlen(buf));

						char (*tokens)[TOKEN_AMOUNT][TOKEN_LENGTH] = malloc(TOKEN_AMOUNT * TOKEN_LENGTH * sizeof(char));
						int tokens_amount = tokenize(tokens, buf);
						convert_tokens_to_nodes(&head, tokens, tokens_amount);
					}
			}
		}
	}
	config->dx = 1;
	config->dy = 1;
	config->num_x = 11;
	config->num_y = 11;
	fclose(f);
	return head;
}

void reshape(GLsizei width, GLsizei height) {
	if (height == 0) height = 1;
	GLfloat aspect = (GLfloat)width / (GLfloat)height;
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, aspect, 0.1f, 100.0f);
}

void init_gl(void) {
   glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
   glClearDepth(1.0f);                   // Set background depth to farthest
   glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
   glDepthFunc(GL_LEQUAL);    // Set the type of depth-test
   glShadeModel(GL_SMOOTH);   // Enable smooth shading
   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Nice perspective corrections
}

void timer(int value) {
	if(redraw) glutPostRedisplay();
	glutTimerFunc(FRAME_DELAY, timer, 0);
}

void keyboard_func(unsigned char Key, int x, int y)
{
	redraw = true;
	switch(Key)
	{
		case 'w':
			offset[0] += 0.5;
			break;
		case 's':
			offset[0] -= 0.5;
			break;
		case 'a':
			offset[1] += 0.5;
			break;
		case 'd':
			offset[1] -= 0.5;
			break;
		case 'r':
			offset[2] += 0.5;
			break;
		case 'f':
			offset[2] -= 0.5;
			break;
		case '+':
			rad -= 0.5;
			break;
		case '-':
			rad += 0.5;
			break;
		default:
			redraw = false;
	}
}

void special_keyboard_func(int key, int x, int y) {
	redraw = true;
	switch(key)
	{
		case GLUT_KEY_UP:
			ang_ver += 0.1;
			break;
		case GLUT_KEY_DOWN:
			ang_ver -= 0.1;
			break;
		case GLUT_KEY_LEFT:
			ang_hor += 0.1;
			break;
		case GLUT_KEY_RIGHT:
			ang_hor -= 0.1;
			break;
		default:
			redraw = false;
	}
}

int main(int argc, char *argv[])
{
	Node *head = parse_config("config");

	config->z_values = malloc(sizeof(double[config->num_x*config->num_y]));
	double x;
	double y;
	int loc_x = 0;
	int loc_y;
	for(x = config->min_x; x <= config->max_x; x+=config->dx) {
		loc_y = 0;
		for(y = config->min_y; y <= config->max_y; y+=config->dy) {
			substitute_variable(head, 'x', x);
			substitute_variable(head, 'y', y);
			(config->z_values)[config->num_x*loc_x+loc_y] = evaluate_tree(head);
			loc_y += 1;
		}
		loc_x += 1;
	}
	free(head);

	//GLUT
	int glut_argc = 1;
	char *glut_argv[1] = {" "};
	glutInit(&glut_argc, glut_argv);
	glutInitWindowSize(config->screen_size_x, config->screen_size_y);
	glutInitDisplayMode(GLUT_DOUBLE);
	glutCreateWindow("Graph3d");
	init_gl();
	glutDisplayFunc(draw_graph);

	glutReshapeFunc(reshape);

	glutKeyboardFunc(keyboard_func);
	glutSpecialFunc(special_keyboard_func);
	glutTimerFunc(0, timer, 0);


	glutMainLoop();

	return 0;
}
