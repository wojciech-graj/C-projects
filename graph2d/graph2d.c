#include "graph2d.h"

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
	int line_width;
	ConfigFunction *func_head;
} ConfigData;

ConfigData *config;

void RenderFloat(float x, float y, void *font, double num, int digits)
{
	char buf[BUFFER_SIZE];
	snprintf(buf, digits + 1, "%f", num);
	glRasterPos2f(x, y);
    glutBitmapString(font, buf);
}

void draw_graph(void) {
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	double x;
	glLineWidth(config->line_width);
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

void parse_config(char *filename)
{
	FILE *f = fopen(filename, "r");
	char buf[BUFFER_SIZE];
	config = malloc(sizeof(ConfigData));
	ConfigFunction *cur_function = NULL;

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
				case 6932: //Xscl
					memmove(buf, buf+5, strlen(buf));
					config->scale_x = atof(buf);
					break;
				case 6996: //Yscl
					memmove(buf, buf+5, strlen(buf));
					config->scale_y = atof(buf);
					break;
				case 6575: //Xdig
					memmove(buf, buf+5, strlen(buf));
					config->digits_x = atof(buf);
					break;
				case 6639: //Ydig
					memmove(buf, buf+5, strlen(buf));
					config->digits_y = atoi(buf);
					break;
				case 7441: //Lnwi .. dth
					memmove(buf, buf+8, strlen(buf));
					config->line_width = atoi(buf);
					break;
				case 5653: //C RE .. D
					cur_function->color = RED;
					break;
				case 5788: //C BL .. UE
					cur_function->color = BLUE;
					break;
				case 5802: //C GR .. EEN
					cur_function->color = GREEN;
					break;
				default:
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
					}
			}
		}
	}
	config->dx = (config->max_x - config->min_x) / config->screen_size_x;
	config->dy = (config->max_y - config->min_y) / config->screen_size_y;
	config->axis_offset = 14 * config->digits_y + 18; // 14:width of character 4:width of dot
	fclose(f);
}

Node *get_nth_func(int n)
{
	int i;
	ConfigFunction *cur_func = config->func_head;
	for(i = 0; i < n; i++)
	{
		cur_func = cur_func->func_next;
	}
	return cur_func->head;
}

void parse_input() {
	char buf[BUFFER_SIZE];
	printf("> ");
	fgets(buf, BUFFER_SIZE, stdin);
	strtok(buf, "\n");
	char *p = strtok(buf, " ");
	if(! strcmp(p, "help")) {
		printf("LIST OF FUNCTIONS:\nexit\nintersect f1 f2 guess iterations\nintegral f right_bound lest_bound\ngradient f x\nmax f left_bound right_bound");
	} else if(! strcmp(p, "exit")) { //Free memory properly
		ConfigFunction *cur_func = config->func_head;
		while(cur_func)
		{
			ConfigFunction *next_func = cur_func->func_next;
			delete_tree(cur_func->head);
			free(cur_func);
			cur_func = next_func;
		}
		free(config);

		exit(0);
	} else if(! strcmp(p, "intersect")) {
		Node *head = malloc(sizeof(Node));
		head->bin_op = *subtract;
		head->un_op = NULL;

		p = strtok(NULL, " ");
		head->node_l = get_nth_func(atoi(p));

		p = strtok(NULL, " ");
		head->node_r = get_nth_func(atoi(p));

		p = strtok(NULL, " ");
		double x = atof(p);

		p = strtok(NULL, " ");

		//calculate intersection using newton's method
		int i;
		for(i = 0; i < atoi(p); i++)
		{
			substitute_variable(head, 'x', x);
			double y = evaluate_tree(head);
			substitute_variable(head, 'x', x + config->dx / 2);
			double gradient = evaluate_tree(head);
			substitute_variable(head, 'x', x - config->dx / 2);
			gradient = (gradient - evaluate_tree(head)) / config->dx;
			x -= y / gradient;
		}

		substitute_variable(head->node_r, 'x', x);
		double y = evaluate_tree(head->node_r);
		printf("x=%f, y=%f\n", x, y);

		free(head);
	} else if(! strcmp(p, "integral")) {
		p = strtok(NULL, " ");
		Node *head = get_nth_func(atoi(p));

		p = strtok(NULL, " ");
		double r_bound = atof(p);

		p = strtok(NULL, " ");
		double l_bound = atof(p);

		double area = 0;
		substitute_variable(head, 'x', l_bound);
		double prev_y = evaluate_tree(head);
		double x;
		for(x = l_bound + config->dx; x <= r_bound; x += config->dx)
		{
			substitute_variable(head, 'x', x);
			double y = evaluate_tree(head);
			area += y + prev_y;
			prev_y = y;
		}
		area *= config->dx / 2;
		printf("%f\n", area);
	} else if(! strcmp(p, "gradient")) {
		p = strtok(NULL, " ");
		Node *head = get_nth_func(atoi(p));

		p = strtok(NULL, " ");
		double x = atof(p);

		substitute_variable(head, 'x', x + config->dx / 2);
		double gradient = evaluate_tree(head);
		substitute_variable(head, 'x', x - config->dx / 2);
		gradient = (gradient - evaluate_tree(head)) / config->dx;

		printf("%f\n", gradient);
	} else if(! strcmp(p, "min")) {
		p = strtok(NULL, " ");
		Node *head = get_nth_func(atoi(p));

		p = strtok(NULL, " ");
		double l_bound = atof(p);

		p = strtok(NULL, " ");
		double r_bound = atof(p);

		substitute_variable(head, 'x', l_bound);

		double min_y = evaluate_tree(head);
		double min_x = l_bound;
		double x;
		for(x = l_bound + config->dx; x <= r_bound; x += config->dx)
		{
			substitute_variable(head, 'x', x);
			double y = evaluate_tree(head);
			if(y < min_y) {
				min_x = x;
				min_y = y;
			}
		}
		printf("(%f, %f)\n", min_x, min_y);
	} else if(! strcmp(p, "max")) {
		p = strtok(NULL, " ");
		Node *head = get_nth_func(atoi(p));

		p = strtok(NULL, " ");
		double l_bound = atof(p);

		p = strtok(NULL, " ");
		double r_bound = atof(p);

		substitute_variable(head, 'x', l_bound);

		double max_y = evaluate_tree(head);
		double max_x = l_bound;
		double x;
		for(x = l_bound + config->dx; x <= r_bound; x += config->dx)
		{
			substitute_variable(head, 'x', x);
			double y = evaluate_tree(head);
			if(y > max_y) {
				max_x = x;
				max_y = y;
			}
		}
		printf("(%f, %f)\n", max_x, max_y);
	}
}

int main(int argc, char *argv[])
{
	parse_config("config");

	//GLUT
	int glut_argc = 1;
	char *glut_argv[1] = {" "};
	glutInit(&glut_argc, glut_argv);
	glutInitWindowSize(config->screen_size_x, config->screen_size_y);
	glutInitDisplayMode(GLUT_RGB | GLUT_SINGLE | GLUT_DEPTH);
	glutCreateWindow("Graph2d");
	gluOrtho2D(config->min_x - config->dx * config->axis_offset, config->max_x, config->min_y - config->dy * config->axis_offset, config->max_y);
	glutDisplayFunc(draw_graph);
	glutIdleFunc(parse_input);

	glutMainLoop();

	return 0;
}
