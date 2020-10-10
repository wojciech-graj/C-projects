#include "graph3d.h"

typedef struct ConfigData {
	int screen_size_x;
	int screen_size_y;
	double min_x;
	double min_y;
	double max_x;
	double max_y;
	double min_z;
	double max_z;
	double dx;
	double dy;
	int num_x;
	int num_y;
	double doffset;
	double dangle;
	double *z_values;
	double axis_x;
	double axis_y;
	double axis_z;
	double blend; //color blend when using adaptive color
	const float *color;
} ConfigData;

ConfigData *config;

bool redraw = true;
float azimuth = 0.0;
float inclination = 0.0;
float radius = 10;
float offset[3] = {0.0, 0.0, 0.0};

void set_rgb(double z)
{
	float inv_blend = 1 / config->blend;
	float frac_color = (z - config->min_z) / (config->max_z - config->min_z);

	double r = inv_blend * frac_color - inv_blend + 1;
	double g = -2 * inv_blend * fabs(frac_color - 0.5) + 1;
	double b = -1 * inv_blend * frac_color + 1;

	r = (r + fabs(r))/2; //only return positive values
	g = (g + fabs(g))/2;
	b = (b + fabs(b))/2;
	glColor3f(r, g, b);
}

void draw(void)
{
	redraw = false;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();
	gluLookAt(radius * sin(inclination) * cos(azimuth) + offset[0],
			radius * sin(azimuth) * sin(inclination) + offset[1],
			radius * cos(inclination) + offset[2],
			offset[0], offset[1],  offset[2],
			0.0f, 0.0f,  1.0f);

	double x;
	double y;
	int loc_x;
	int loc_y;
	if(config->blend == DBL_MAX) glColor3f(config->color[0], config->color[1], config->color[2]);

	loc_x = 0;
	for(x = config->min_x; x <= config->max_x; x+=config->dx)
	{
		loc_y = 0;
		glBegin(GL_LINE_STRIP);
			for(y = config->min_y; y <= config->max_y; y+=config->dy) {
				double z = config->z_values[config->num_x*loc_x+loc_y];
				if(config->blend != DBL_MAX) set_rgb(z);
				glVertex3f(x, y, z);
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
				double z = config->z_values[config->num_x*loc_x+loc_y];
				if(config->blend != DBL_MAX) set_rgb(z);
				glVertex3f(x, y, z);
				loc_x += 1;
			}
		glEnd();
		loc_y += 1;
	}

	glColor3f(1, 1, 1);
	glBegin(GL_LINES);
		if(config->axis_x != DBL_MAX) {
			glVertex3f(config->min_x, config->axis_y, config->axis_z);
			glVertex3f(config->max_x, config->axis_y, config->axis_z);
			glVertex3f(config->axis_x, config->min_y, config->axis_z);
			glVertex3f(config->axis_x, config->max_y, config->axis_z);
			glVertex3f(config->axis_x, config->axis_y, config->min_z);
			glVertex3f(config->axis_x, config->axis_y, config->max_z);
		}
	glEnd();

	glutSwapBuffers();
}

Node *parse_config(char *filename)
{
	FILE *f = fopen(filename, "r");
	char buf[BUFFER_SIZE];
	config = malloc(sizeof(ConfigData));
	Node *head;

	//set default values
	config->doffset = 1;
	config->dangle = M_PI / 6;
	config->color = WHITE;
	config->blend = DBL_MAX;
	config->axis_x = DBL_MAX;
	config->axis_y = DBL_MAX;
	config->axis_z = DBL_MAX;
	config->min_z = DBL_MAX;
	config->max_z = -DBL_MAX;

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
				case 4663: //dAng .. le
					memmove(buf, buf+7, strlen(buf));
					config->dangle = atof(buf);
					break;
				case 5046: //dOff .. set
					memmove(buf, buf+8, strlen(buf));
					config->doffset = atof(buf);
					break;
				case 6537: //Xaxi .. s
					memmove(buf, buf+6, strlen(buf));
					config->axis_x = atof(buf);
					break;
				case 6601: //Yaxi .. s
					memmove(buf, buf+6, strlen(buf));
					config->axis_y = atof(buf);
					break;
				case 6409: //Zaxi .. s
					memmove(buf, buf+6, strlen(buf));
					config->axis_z = atof(buf);
					break;
				case 5653: //C RE .. D
					config->color = RED;
					break;
				case 5788: //C BL .. UE
					config->color = BLUE;
					break;
				case 5802: //C GR .. EEN
					config->color = GREEN;
					break;
				case 5772:// C AD ..  APTIVE
					memmove(buf, buf+11, strlen(buf));
					config->blend = atof(buf);
					break;
				default:
					if(buf[0] == 'F') {
						memmove(buf, buf+2, strlen(buf));

						char (*tokens)[TOKEN_AMOUNT][TOKEN_LENGTH] = malloc(TOKEN_AMOUNT * TOKEN_LENGTH * sizeof(char));
						int tokens_amount = tokenize(tokens, buf);
						convert_tokens_to_nodes(&head, tokens, tokens_amount);
					} else if(buf[0] == 'd') {
						if(buf[1] == 'X') {
							memmove(buf, buf+3, strlen(buf));
							config->dx = atof(buf);
						} else if(buf[1] == 'Y') {
							memmove(buf, buf+3, strlen(buf));
							config->dy = atof(buf);
						}
					}
			}
		}
	}
	config->num_x = abs(config->max_x - config->min_x) / config->dx + 1;
	config->num_y = abs(config->max_y - config->min_y) / config->dy + 1;
	//set offset to middle of graph
	offset[0] = (config->max_x + config->min_x) / 2;
	offset[1] = (config->max_y + config->min_y) / 2;
	fclose(f);
	return head;
}

void reshape(GLsizei width, GLsizei height)
{
	if (height == 0) height = 1;
	GLfloat aspect = (GLfloat)width / (GLfloat)height;
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, aspect, 0.1f, 100.0f);
}

void init_gl(void)
{
   glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
   glClearDepth(1.0f);
   glEnable(GL_DEPTH_TEST);
   glDepthFunc(GL_LEQUAL);
   glShadeModel(GL_SMOOTH);
   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

void timer(int value)
{
	if(redraw) glutPostRedisplay();
	glutTimerFunc(FRAME_DELAY, timer, 0);
}

void keyboard_func(unsigned char Key, int x, int y)
{
	redraw = true;
	switch(Key)
	{
		case 'w':
			offset[0] -= config->doffset * cos(azimuth);
			offset[1] -= config->doffset * sin(azimuth);
			break;
		case 's':
			offset[0] += config->doffset * cos(azimuth);
			offset[1] += config->doffset * sin(azimuth);
			break;
		case 'd':
			offset[0] -= config->doffset * sin(azimuth);
			offset[1] += config->doffset * cos(azimuth);
			break;
		case 'a':
			offset[0] += config->doffset * sin(azimuth);
			offset[1] -= config->doffset * cos(azimuth);
			break;
		case 'r':
			offset[2] += config->doffset;
			break;
		case 'f':
			offset[2] -= config->doffset;
			break;
		case '+':
			radius -= config->doffset;
			break;
		case '-':
			radius += config->doffset;
			break;
		default:
			redraw = false;
	}
}

void special_keyboard_func(int key, int x, int y)
{
	redraw = true;
	switch(key)
	{
		case GLUT_KEY_UP:
			inclination += config->dangle;
			break;
		case GLUT_KEY_DOWN:
			inclination -= config->dangle;
			break;
		case GLUT_KEY_LEFT:
			azimuth += config->dangle;
			break;
		case GLUT_KEY_RIGHT:
			azimuth -= config->dangle;
			break;
		default:
			redraw = false;
	}
}

void free_memory(void)
{
	free(config->z_values);
	free(config);
}

int main(int argc, char *argv[])
{
	Node *head = parse_config("config");

	//calculate all points on graph
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
			double z = evaluate_tree(head);
			config->z_values[config->num_x*loc_x+loc_y] = z;
			if(z > config->max_z) config->max_z = z;
			if(z < config->min_z) config->min_z = z;
			loc_y += 1;
		}
		loc_x += 1;
	}
	delete_tree(head);

	//GLUT
	int glut_argc = 1;
	char *glut_argv[1] = {" "};
	glutInit(&glut_argc, glut_argv);
	glutInitWindowSize(config->screen_size_x, config->screen_size_y);
	glutInitDisplayMode(GLUT_DOUBLE);
	glutCreateWindow("Graph3d");
	init_gl();
	glutDisplayFunc(draw);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard_func);
	glutSpecialFunc(special_keyboard_func);
	glutCloseFunc(free_memory);
	glutTimerFunc(0, timer, 0);

	glutMainLoop();

	return 0;
}
