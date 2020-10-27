#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <assert.h>

SDL_Window *window;
SDL_GLContext main_context;

void init()
{
    assert(SDL_Init(SDL_INIT_EVERYTHING) == 0);

	window = SDL_CreateWindow("MARBLE GAME",
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		640, 480, SDL_WINDOW_OPENGL);
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
}

void quit()
{
	SDL_GL_DeleteContext(main_context);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

int main(int argc, char *argv[])
{
	init();

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBegin(GL_TRIANGLES);
		glColor3f(1, 0, 0);
		glVertex2f(0, 0);
		glColor3f(0, 1, 0);
		glVertex2f(0, 1);
		glColor3f(0, 0, 1);
		glVertex2f(1, 1);
	glEnd();

	SDL_GL_SwapWindow(window);

	sleep(5);

	quit();
}
