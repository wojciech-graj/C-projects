#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <assert.h>
#include <math.h>

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 640
#define level_width 5
#define level_height 10
#define MIN_HEIGHT -2
#define M_TAO (M_PI * 2)

SDL_Window *window;
SDL_GLContext main_context;

int cur_row = 0;

float level[][4] = {
	{3, 3, 3, 3}, {3, 3, 3, 3}, {3, 3, 3, 3}, {3, 3, 3, 3}, {3, 3, 3, 3},
			{3, 3, 3, 3}, {5, 5, 5, 5}, {3, 3, 3, 3}, {3, 3, 3, 3}, {3, 3, 4, 3},
	{3, 3, 3, 3}, {3, 5, 5, 3}, {3, 3, 3, 3}, {6, 6, 6, 6}, {4, 3, 3, 3},
			{3, 3, 3, 3}, {3, 3, 3, 3}, {5, 5, 5, 5}, {3, 3, 3, 3}, {3, 3, 3, 3},
	{3, 3, 3, 3}, {3, 3, 3, 3}, {3, 3, 3, 3}, {3, 3, 3, 3}, {3, 3, 3, 3},
		{3, 3, 3, 3}, {3, 3, 3, 3}, {3, 3, 3, 3}, {3, 3, 3, 3}, {3, 3, 3, 3},
	{3, 3, 3, 3}, {3, 3, 3, 3}, {3, 3, 3, 3}, {3, 3, 3, 3}, {3, 3, 3, 3},
		{3, 3, 3, 3}, {3, 3, 3, 3}, {3, 3, 3, 3}, {3, 3, 3, 3}, {3, 3, 3, 3},
	{3, 3, 3, 3}, {3, 3, 3, 3}, {3, 3, 3, 3}, {3, 3, 3, 3}, {3, 3, 3, 3},
		{3, 3, 3, 3}, {3, 3, 3, 3}, {3, 3, 3, 3}, {3, 3, 3, 3}, {3, 3, 3, 3}};

float ball_pos[2];
float frac_ball_pos[2];
int ball_tile;

float level_projection[level_width * level_height][4];

int num_ball_points = 12;
float ball_rad = .2;

enum directions{l, t, r, b};

const float RED[] = {1, 0, 0};
const float GREEN[] = {0, 1, 0};
const float BLUE[] = {0, 0, 1};

void init(void)
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

void quit(void)
{
	SDL_GL_DeleteContext(main_context);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void draw_side(float x_m, float x_s, float t_b, float t_s, float b_t, float b_s, const float *color)
{
	if(t_b != b_s || t_s != b_t) {
		glColor3fv(color);
		glBegin(GL_QUADS);
		glVertex2f(x_m, t_b);
		glVertex2f(x_s, t_s);
		glVertex2f(x_s, b_t);
		glVertex2f(x_m, b_s);
		glEnd();
	}
}

void draw(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//draw board
	int x, y;
	for(y = 0; y < level_height; y++)
	{
		int offset = y % 2;
		glColor3f(1, 1, offset);
		for(x = 0; x < level_width; x++)
		{
			int tile_i = y * level_width + x;
			float *tile = level_projection[tile_i];
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
			float *tile_bl = level_projection[(y + 1) * level_width + x + offset - 1];
			if(x == 0 && ! offset || y == level_height - 1) {
				draw_side(x_m, x_l, tile[b], tile[l], MIN_HEIGHT, MIN_HEIGHT, BLUE);
			} else {
				draw_side(x_m, x_l, tile[b], tile[l], tile_bl[t], tile_bl[r], BLUE);
			}

			float *tile_br = level_projection[(y + 1) * level_width + x + offset];
			if(x == level_width - 1 && offset || y == level_height - 1) {
				draw_side(x_m, x_r, tile[b], tile[r], MIN_HEIGHT, MIN_HEIGHT, RED);
			} else {
				draw_side(x_m, x_r, tile[b], tile[r], tile_br[t], tile_br[l], RED);
			}

			//tile outline
			glColor3f(1, 1, 1);
			glBegin(GL_LINE_LOOP);
			glVertex2f(x_l, tile[l]);
			glVertex2f(x_m, tile[t]);
			glVertex2f(x_r, tile[r]);
			glVertex2f(x_m, tile[b]);
			glEnd();

			//draw ball
			if(ball_tile == tile_i) {
				float angle;
				glColor3fv(GREEN);
				glBegin(GL_POLYGON);
				for(angle = 0; angle < M_TAO; angle += M_TAO / num_ball_points)
				{

					glVertex2f(ball_rad * cos(angle) + x_l + (x_r - x_l) * frac_ball_pos[0],
						ball_rad * sin(angle) + ball_rad + tile[b] + (tile[t] - tile[b]) * (-frac_ball_pos[1] + 1));
				}
				glEnd();
			}
		}
	}

	SDL_GL_SwapWindow(window);
}

void calculate_projection(void)
{
	int y, x;
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

//TODO: OPTIMIZE... there ought to exist some one equation for this
void calculate_ball_tile(void)
{
	int tile_x = round(ball_pos[0]);
	int tile_y = round(ball_pos[1]);
	float sum = ball_pos[0] + ball_pos[1];
	ball_tile = 2 * tile_y * level_width + tile_x;
	frac_ball_pos[0] = ball_pos[0] + .5 - tile_x;
	frac_ball_pos[1] = ball_pos[1] + .5 - tile_y;
	if(sum < tile_x + tile_y - .5) {//TL
		ball_tile -= level_width + 1;
		frac_ball_pos[0] += .5;
		frac_ball_pos[1] += .5;
	} else if(sum > tile_x + tile_y + .5) {//BR
		ball_tile += level_width;
		frac_ball_pos[0] -= .5;
		frac_ball_pos[1] -= .5;
	} else if(ball_pos[0] < tile_x
		&& ball_pos[1] > tile_y
		&& ball_pos[1] - tile_y > ball_pos[0] - tile_x + .5) { //BL
		ball_tile += level_width - 1;
		frac_ball_pos[0] += .5;
		frac_ball_pos[1] -= .5;
	} else if(ball_pos[0] > tile_x
		&& ball_pos[1] < tile_y
		&& ball_pos[1] - tile_y + .5 < ball_pos[0] - tile_x) { //TR
		ball_tile -= level_width;
		frac_ball_pos[0] -= .5;
		frac_ball_pos[1] += .5;
	}
}

int main(int argc, char *argv[])
{
	init();

	calculate_projection();

	ball_pos[0] = 0;
	ball_pos[1] = 0;
	calculate_ball_tile();
	draw();

	SDL_Event event;
	while(1)
	{
		while (SDL_WaitEvent(&event) >= 0)
		{
			if (event.type == SDL_QUIT) {
				quit();
				return 0;
			} else if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym)
			    {
			        case SDLK_LEFT:  ball_pos[0] -= .3; break;
			        case SDLK_RIGHT: ball_pos[0] += .3; break;
			        case SDLK_UP:    ball_pos[1] -= .3; break;
			        case SDLK_DOWN:  ball_pos[1] += .3; break;
			    }
				calculate_ball_tile();
				draw();
			}
		}
	}
	quit();
	return 0;

}
