#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <assert.h>
#include <math.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define level_width 4
#define level_height 4

SDL_Window *window;
SDL_GLContext main_context;

int cur_row = 0;

int level[][4] = {
	{3, 3, 3, 3}, {3, 3, 3, 3}, {3, 3, 3, 3}, {3, 3, 3, 3},
			{3, 3, 3, 3}, {3, 3, 3, 3}, {3, 3, 3, 3}, {3, 3, 3, 3},
	{3, 3, 3, 3}, {3, 5, 5, 3}, {3, 3, 3, 3}, {6, 6, 6, 6},
			{3, 3, 3, 3}, {3, 3, 3, 3}, {3, 3, 3, 3}, {3, 3, 3, 3}};

float level_projection[level_width * level_height][4];

enum dir{l, t, r, b};

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
	for(y = 0; y < level_height; y++)
	{
		int offset = y % 2;
		glColor3f(1, 1, offset);
		for(x = 0; x < level_width; x++)
		{
			float *tile = level_projection[y * level_width + x];
			float x_l = x - .5 + offset/2.;
			float x_m = x + offset/2.;
			float x_r = x + .5 + offset/2.;

			int slope_sign = (signbit(tile[l] - tile[r]) ?  -1 : 1);

			//top surface
			float c_l = .5 + ((tile[l] - tile[b]) / 3) * slope_sign; //color
			float c_r = .5 + ((tile[t] - tile[r]) / 3) * slope_sign;
			glBegin(GL_TRIANGLES);
			glColor3f(c_l, c_l, c_l);
			triangle(x_m, tile[b],
				x_l, tile[l],
				x_m, tile[t]); //left triangle
			glColor3f(c_r, c_r, c_r);
			triangle(x_m, tile[b],
				x_r, tile[r],
				x_m, tile[t]); // right triangle
			glEnd();

			//outline
			glColor3f(0, 0, 0);
			glBegin(GL_LINE_LOOP);
			quad(x_l, tile[l],
				x_m, tile[t],
				x_r, tile[r],
				x_m, tile[b]);
			glEnd();

			//
		}
	}
	SDL_GL_SwapWindow(window);
}

void calculate_projection()
{
	int y,x;
	for(y = 0; y < level_height; y++)
	{
		for(x = 0; x < level_width; x++)
		{
			int i = y * level_width + x;
			level_projection[i][l] = level[i][l]/2. - y/4.;
			level_projection[i][t] = level[i][t]/2. - y/4. + .25;
			level_projection[i][r] = level[i][r]/2. - y/4.;
			level_projection[i][b] = level[i][b]/2. - y/4. - .25;
		}
	}
}

int main(int argc, char *argv[])
{
	init();

	calculate_projection();

	draw();

	sleep(30);

	quit();
}
