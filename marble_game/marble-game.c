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
	0, 4, 3, 8, 8, 3,
	  4, 3, 8, 8, 8, 3,
	4, 4, 3, 8, 8, 3,
	  4, 3, 3, 3, 3, 3,
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
	gluOrtho2D(-1, 7, -1, 7);
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

void quad(float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4)
{
	glVertex2f(x1, y1);
	glVertex2f(x2, y2);
	glVertex2f(x3, y3);
	glVertex2f(x4, y4);
}

void draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	int x, y;
	for(y = 1; y < level_height - 1; y++)
	{
		int offset = y % 2;
		glColor3f(1, 1, offset);
		for(x = 0; x < level_width - 1; x++)
		{
			int sq_l = level[y * level_width + x];
			int sq_r = level[y * level_width + x + 1];
			int sq_t = level[(y - 1) * level_width + x + offset];
			int sq_b = level[(y + 1) * level_width + x + offset];
			if(sq_t - sq_b >= 2) sq_t = sq_b;
			if(sq_b - sq_t >= 2) sq_b = sq_t;
			if(sq_l - sq_r >= 2) sq_l = sq_t;
			if(sq_r - sq_l >= 2) sq_r = sq_t;
			float x_l = x - .5 + offset/2.;
			float x_m = x + offset/2.;
			float x_r = x + .5 + offset/2.;
			float y_l = sq_l/2. - y/4.;
			float y_t = sq_t/2. - y/4. + .25;
			float y_r = sq_r/2. - y/4.;
			float y_b = sq_b/2. - y/4. - .25;

			int slope_sign = (signbit(y_l - y_r) ?  -1 : 1);

			float c_l = .5 + ((y_l - y_b) / 3) * slope_sign; //color
			float c_r = .5 + ((y_t - y_r) / 3) * slope_sign;

			glBegin(GL_TRIANGLES);
			glColor3f(c_l, c_l, c_l);
			triangle(x_m, y_b,
				x_l, y_l,
				x_m, y_t); //left triangle
			glColor3f(c_r, c_r, c_r);
			triangle(x_m, y_b,
				x_r, y_r,
				x_m, y_t); // right triangle
			glEnd();

			glColor3f(0, 0, 0);
			glBegin(GL_LINE_LOOP);
			quad(x_l, y_l,
				x_m, y_t,
				x_r, y_r,
				x_m, y_b);
			glEnd();

			if(y < level_height - 3)
			{
				if(sq_r - level[(y+2) * level_width + x + 1] >= 2) {
					glColor3f(0, 1, 0);
					glBegin(GL_QUADS);
					quad(x_r, y_r,
						x_m, y_b,
						x_m, level[(y + 3) * level_width + x + offset]/2. - y/4. - .25,
						x_r, level[(y + 2) * level_width + x + 1]/2. - y/4.);
					glEnd();
				}
				if(sq_l - level[(y+2) * level_width + x] >= 2) {
					glColor3f(0, 1, 1);
					glBegin(GL_QUADS);
					quad(x_l, y_l,
						x_m, y_b,
						x_m, level[(y + 3) * level_width + x+ offset]/2. - y/4. - .25,
						x_l, level[(y + 2) * level_width + x - 1]/2. - y/4.);
					glEnd();
				}
			}
		}
	}
	SDL_GL_SwapWindow(window);
}

int main(int argc, char *argv[])
{
	init();

	draw();

	sleep(30);

	quit();
}
