#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <assert.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480

SDL_Window *window;
SDL_GLContext main_context;

int cur_row = 0;

int level[] = {
	3, 3, 3, 3, 3, 3,
	  3, 3, 3, 3, 3, 3,
	3, 3, 4, 4, 3, 3,
	  3, 3, 4, 4, 3, 3,
	3, 3, 3, 3, 3, 3,
	  3, 3, 3, 3, 3, 3,
	3, 3, 3, 3, 3, 3};

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
	gluOrtho2D(0, 10, 0, 10);
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
	glBegin(GL_TRIANGLES);
		for(y = 1; y < level_height - 1; y++)
			for(x = 1; x < level_width - 1; x++)
			{
				glColor3f(0, 1, y % 2);
				int x_offset = (y % 2 == 0); //TODO:Rewrite better
				glVertex2f(x - .5 + x_offset/ 2., level[y * level_width + x + x_offset - 1] - y/4.); //L
				glVertex2f(x + x_offset/ 2., level[(y - 1) * level_width + x] - y/4. + .25); //T
				glVertex2f(x + x_offset/ 2., level[(y + 1) * level_width + x] - y/4. - .25); //B

				glVertex2f(x + x_offset/ 2., level[(y - 1) * level_width + x] - y/4. + .25); //T
				glVertex2f(x + x_offset/ 2., level[(y + 1) * level_width + x] - y/4. - .25); //B
				glVertex2f(x + .5 + x_offset/ 2., level[y * level_width + x + x_offset] - y/4.); //R

			}
	glEnd();

	SDL_GL_SwapWindow(window);
}

int main(int argc, char *argv[])
{
	init();

	draw();

	sleep(10);

	quit();
}
