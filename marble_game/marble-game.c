#include "marble-game.h"

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
				float ball_x = (x_r - x_l) * frac_ball_pos[0];
				float ball_y = (tile[t] - tile[b]) * (.5 - frac_ball_pos[1]);
				if(frac_ball_pos[0] < 0.5 && tile[l] != tb_avg) {
					ball_y += (tb_avg - tile[l]) * 2 * (frac_ball_pos[0]) + tile[l];
				} else if(frac_ball_pos[0] >= 0.5 && tile[r] != tb_avg) {
					ball_y += (tile[r] - tb_avg) * 2 * (frac_ball_pos[0]) + tile[l];
				} else {
					ball_y += tb_avg;
				}

				glColor3fv(GREEN);
				glBegin(GL_POLYGON);
				for(angle = 0; angle < M_TAO; angle += M_TAO / num_ball_points)
				{
					glVertex2f(ball_rad * cos(angle) + x_l + ball_x,
						ball_rad * sin(angle) + ball_rad + ball_y);
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

//TODO:
//OPTIMIZE: there ought to exist some one equation for this
//ADD: Testing for edge cases
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

int process_input(void *ptr)
{
	(void) ptr;
	SDL_Event event;

	while(1)
	{
		while (SDL_WaitEvent(&event) >= 0)
		{
			if (event.type == SDL_QUIT) {
				quit();
				exit(0);
			} else if (event.type == SDL_KEYDOWN) {
				switch (event.key.keysym.sym)
				{
					case SDLK_LEFT:
						ball_vel[0] -= .01;
						break;
					case SDLK_RIGHT:
						ball_vel[0] += .01;
						break;
					case SDLK_UP:
						ball_vel[1] -= .01;
						break;
					case SDLK_DOWN:
						ball_vel[1] += .01;
						break;
				}
			}
		}
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

	SDL_Thread *input_thread = SDL_CreateThread(process_input, "input_thread", (void *)NULL);
	assert(input_thread);

	while(1)
	{
		ball_pos[0] += ball_vel[0];
		ball_pos[1] += ball_vel[1];
		calculate_ball_tile();
		draw();
		SDL_Delay(1000. / 30.);
	}
	quit();
	return 0;

}
