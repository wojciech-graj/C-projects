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
			{3, 3, 3, 3}, {5, 5, 5, 5}, {3, 3, 3, 3}, {3, 3, 3, 3},
	{3, 3, 3, 3}, {3, 5, 5, 3}, {3, 3, 3, 3}, {6, 6, 6, 6},
			{3, 3, 3, 3}, {3, 3, 3, 3}, {5, 5, 5, 5}, {3, 3, 3, 3}};

float level_projection[level_width * level_height][4];

enum directions{l, t, r, b};

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
			float tb_avg = (tile[b] + tile[t])/2.;

			//top surface
			float c_l = .5 + fabs((tile[l] - tb_avg) / 3); //color
			float c_r = .5 + fabs((tile[r] - tb_avg) / 3);
			glBegin(GL_TRIANGLES);
			//left triangle
			glColor3f(c_l, c_l, c_l);
			glVertex2f(x_m, tile[b]);
			glVertex2f(x_l, tile[l]);
			glVertex2f(x_m, tile[t]);
			// right triangle
			glColor3f(c_r, c_r, c_r);
			glVertex2f(x_m, tile[b]);
			glVertex2f(x_r, tile[r]);
			glVertex2f(x_m, tile[t]);
			glEnd();

			//side fill
			float *tile_br = level_projection[(y + 1) * level_width + x + offset];
			float *tile_bl = level_projection[(y + 1) * level_width + x + offset - 1];
			float br_t = tile_br[t];
			float br_l = tile_br[l];
			float bl_t = tile_bl[t];
			float bl_r = tile_bl[r];
			if(x == 0 && ! offset || y == level_height - 1) {
				bl_t = -2;
				bl_r = -2;
			}
			if(x == level_width - 1 && offset || y == level_height - 1) {
				br_t = -2;
				br_l = -2;
			}
			if(tile[b] != br_t || tile[r] != br_t) {
				glColor3f(0, 0, .7);
				glBegin(GL_QUADS);
				glVertex2f(x_m, tile[b]);
				glVertex2f(x_r, tile[r]);
				glVertex2f(x_r, br_t);
				glVertex2f(x_m, br_l);
				glEnd();
			}
			if(tile[b] != bl_r || tile[l] != bl_t) {
				glColor3f(0, .7, 0);
				glBegin(GL_QUADS);
				glVertex2f(x_m, tile[b]);
				glVertex2f(x_l, tile[l]);
				glVertex2f(x_l, bl_t);
				glVertex2f(x_m, bl_r);
				glEnd();
			}

			//outline
			glColor3f(1, 1, 1);
			glBegin(GL_LINE_LOOP);
			glVertex2f(x_l, tile[l]);
			glVertex2f(x_m, tile[t]);
			glVertex2f(x_r, tile[r]);
			glVertex2f(x_m, tile[b]);
			glEnd();
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
