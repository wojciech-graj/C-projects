#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <assert.h>
#include <math.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

SDL_Window *window;
SDL_GLContext main_context;

int cur_row = 0;

int level[] = {
	0, 5, 5, 5, 5, 5,
	  4, 4, 4, 4, 4, 4,
	4, 4, 4, 4, 4, 4,
	  4, 3, 3, 3, 3, 4,
	3, 3, 3, 3, 3, 3,
	  3, 3, 3, 3, 3, 3,
	0, 3, 3, 3, 3, 3};

int level_width = 6;
int level_height = 7;

void init()
{
    assert(SDL_Init(SDL_INIT_EVERYTHING) == 0);

	window = SDL_CreateWindow("MARBLE GAME",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		WINDOW_WIDTH, WINDOW_HEIGHT,
		SDL_WINDOW_OPENGL);
	assert(window);

	main_context = SDL_GL_CreateContext(window);
	assert(main_context);

	assert(SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE) == 0);
	assert(SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1) == 0);

    glClearDepth(1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glShadeModel(GL_SMOOTH);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	gluOrtho2D(-1, 5, -1, 5);
}

void quit()
{
	SDL_GL_DeleteContext(main_context);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void triangle(float x1, float y1, float x2, float y2, float x3, float y3)
{
	glVertex2f(x1, y1);
	glVertex2f(x2, y2);
	glVertex2f(x3, y3);
}

void draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	int x, y;
	for(y = 1; y < level_height - 1; y++)
	{
		int offset = y % 2;
		glColor3f(1, 1, offset);
		for(x = 0; x < level_width; x++)
		{
			float x_vals[] = {
				x - .5 + offset/2., //Left
				x + offset/2., //middle
				x + .5 + offset/2.}; //right
			float y_vals[] = {
				level[y * level_width + x]/2. - y/4., //left
				level[(y - 1) * level_width + x + offset]/2. - y/4. + .25, //top
				level[y * level_width + x + 1]/2. - y/4., //right
				level[(y + 1) * level_width + x + offset]/2. - y/4. - .25}; //bottom
			int slope_sign = (signbit(y_vals[0] - y_vals[2]) ?  -1 : 1);
			float l_r_avg = (y_vals[0] + y_vals[2]) / 2;

			glBegin(GL_TRIANGLES);
			double cmul = ((y_vals[1] - y_vals[2]) / 4) * slope_sign; //color multiplier

			glColor3f(.5 + cmul, .5 + cmul, .5 + cmul);
			triangle(x_vals[0], y_vals[0], x_vals[2], y_vals[2], x_vals[1], y_vals[1]); //top triangle

			cmul = ((y_vals[0] - y_vals[3]) / 4) * slope_sign;
			glColor3f(.5 + cmul, .5 + cmul, .5 + cmul);
			triangle(x_vals[0], y_vals[0], x_vals[2], y_vals[2], x_vals[1], y_vals[3]); // bottom triangle
			glEnd();

			glColor3f(0, 0, 0);
			glBegin(GL_LINE_STRIP);
			glVertex2f(x_vals[0], y_vals[0]); //L
			glVertex2f(x_vals[1], y_vals[1]); //T
			glVertex2f(x_vals[2], y_vals[2]); //R
			glVertex2f(x_vals[1], y_vals[3]); //B
			glEnd();
		}
	}
	SDL_GL_SwapWindow(window);
}

int main(int argc, char *argv[])
{
	init();

	draw();

	sleep(20);

	quit();
}
