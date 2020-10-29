#include "marble-game.h"

typedef struct Marble {
	float position[2];
	int tile;
	float tile_position[2];
	float velocity[2];
	float radius;
	void (*physics_process)(Marble*);
} Marble;

void init_sdl(void)
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

	keystates = SDL_GetKeyboardState(NULL);
	assert(keystates);

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
	int tile_position[2];
	for(tile_position[y] = 0; tile_position[y] < level_height; tile_position[y]++)
	{
		int offset = tile_position[y] % 2;
		glColor3f(1, 1, offset);
		for(tile_position[x] = 0; tile_position[x] < level_width; tile_position[x]++)
		{
			int tile_index = tile_position[y] * level_width + tile_position[x];
			float *tile = level_projection[tile_index];
			float x_l = tile_position[x] - .5 + offset/2.;
			float x_m = tile_position[x] + offset/2.;
			float x_r = tile_position[x] + .5 + offset/2.;
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
			float *tile_bl = level_projection[(tile_position[y] + 1) * level_width + tile_position[x] + offset - 1];
			if(tile_position[x] == 0 && ! offset || tile_position[y] == level_height - 1) {
				draw_side(x_m, x_l, tile[b], tile[l], MIN_HEIGHT, MIN_HEIGHT, BLUE);
			} else {
				draw_side(x_m, x_l, tile[b], tile[l], tile_bl[t], tile_bl[r], BLUE);
			}

			float *tile_br = level_projection[(tile_position[y] + 1) * level_width + tile_position[x] + offset];
			if(tile_position[x] == level_width - 1 && offset || tile_position[y] == level_height - 1) {
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
			if(player_marble->tile == tile_index) {
				float angle;
				float ball_x = (x_r - x_l) * player_marble->tile_position[0];
				float ball_y = (tile[t] - tile[b]) * (.5 - player_marble->tile_position[y]);
				if(player_marble->tile_position[x] < 0.5 && tile[l] != tb_avg) {
					ball_y += 2 * (tb_avg - tile[l]) * (player_marble->tile_position[x]) + tile[l];
				} else if(player_marble->tile_position[x] >= 0.5 && tile[r] != tb_avg) {
					ball_y += 2 * (tb_avg - tile[r]) * (1 - player_marble->tile_position[x]) + tile[r];
				} else {
					ball_y += tb_avg;
				}

				glColor3fv(GREEN);
				glBegin(GL_POLYGON);
				for(angle = 0; angle < M_TAO; angle += M_TAO / NUM_CIRCLE_POINTS)
				{
					glVertex2f(player_marble->radius * cos(angle) + x_l + ball_x,
						player_marble->radius * sin(angle) + player_marble->radius + ball_y);
				}
				glEnd();
			}
		}
	}
	SDL_GL_SwapWindow(window);
}

void calculate_projection(void)
{
	int tile_position[2];
	for(tile_position[y] = 0; tile_position[y] < level_height; tile_position[y]++)
	{
		for(tile_position[x] = 0; tile_position[x] < level_width; tile_position[x]++)
		{
			int i = tile_position[y] * level_width + tile_position[x];
			level_projection[i][l] = level[i][l]/2. - tile_position[y]/4.;
			level_projection[i][t] = level[i][t]/2. - tile_position[y]/4. + .25;
			level_projection[i][r] = level[i][r]/2. - tile_position[y]/4.;
			level_projection[i][b] = level[i][b]/2. - tile_position[y]/4. - .25;
		}
	}
}

void physics_process_marble(Marble *marble)
{
	//TODO: OPTIMIZE: there ought to exist some one equation for this
	//calculate marble->tile and marble->tile_position
	int tile_x = round(marble->position[x]);
	int tile_y = round(marble->position[y]);
	float sum = marble->position[0] + marble->position[y];
	marble->tile = 2 * tile_y * level_width + tile_x;
	marble->tile_position[x] = marble->position[x] + .5 - tile_x;
	marble->tile_position[y] = marble->position[y] + .5 - tile_y;
	if(sum < tile_x + tile_y - .5) {//TL
		marble->tile -= level_width + 1;
		marble->tile_position[x] += .5;
		marble->tile_position[y] += .5;
	} else if(sum > tile_x + tile_y + .5) {//BR
		marble->tile += level_width;
		marble->tile_position[x] -= .5;
		marble->tile_position[y] -= .5;
	} else if(marble->position[x] < tile_x
		&& marble->position[y] > tile_y
		&& marble->position[y] - tile_y > marble->position[0] - tile_x + .5) { //BL
		marble->tile += level_width - 1;
		marble->tile_position[x] += .5;
		marble->tile_position[y] -= .5;
	} else if(marble->position[x] > tile_x
		&& marble->position[y] < tile_y
		&& marble->position[y] - tile_y + .5 < marble->position[0] - tile_x) { //TR
		marble->tile -= level_width;
		marble->tile_position[x] -= .5;
		marble->tile_position[y] += .5;
	}

	//calculate marble->velocity
	float *tile = level[marble->tile];
	float tb_avg = (tile[t] + tile[b])/2.;
	if(marble->tile_position[x] <= .5 && tile[l] != tb_avg) {
		marble->velocity[x] += (tile[l] - tb_avg) * GRAVITY_ACCELERATION;
	} else if(marble->tile_position[x] > .5 && tile[r] != tb_avg) {
		marble->velocity[x] += (tb_avg - tile[r]) * GRAVITY_ACCELERATION;
	}
	if(tile[t] != tile[b]) {
		marble->velocity[y] += (tile[t] - tile[b])/2. * GRAVITY_ACCELERATION;
	}
	marble->velocity[x] -= FRICTION * marble->velocity[x];
	marble->velocity[y] -= FRICTION * marble->velocity[y];

	//calculate marble->position
	marble->position[x] += marble->velocity[x];
	marble->position[y] += marble->velocity[y];
}

void init_marble(Marble **marble)
{
	*marble = malloc(sizeof(Marble));
	(*marble)->position[x] = 0;
	(*marble)->position[y] = 0;
	(*marble)->velocity[x] = 0;
	(*marble)->velocity[y] = 0;
	(*marble)->radius = .2;
	(*marble)->physics_process = &physics_process_marble;
}

void process_input(void)
{
	SDL_Event event;

	while (SDL_PollEvent(&event) > 0)
	{
		if (event.type == SDL_QUIT) {
			quit();
			exit(0);
		}
	}

    if(keystates[SDL_SCANCODE_LEFT]) {
		player_marble->velocity[x] -= MARBLE_ACCELERATION;
    }
    if(keystates[SDL_SCANCODE_RIGHT]) {
		player_marble->velocity[x] += MARBLE_ACCELERATION;
    }
    if(keystates[SDL_SCANCODE_UP]) {
		player_marble->velocity[y] -= MARBLE_ACCELERATION;
    }
    if(keystates[SDL_SCANCODE_DOWN]) {
		player_marble->velocity[y] += MARBLE_ACCELERATION;
    }
}

int main(int argc, char *argv[])
{
	init_sdl();

	calculate_projection();

	init_marble(&player_marble);
	assert(player_marble);

	while(true)
	{
		int frame_start = SDL_GetTicks();
		process_input();
		player_marble->physics_process(player_marble);
		draw();
		int frame_time = SDL_GetTicks() - frame_start;
		SDL_Delay(FRAMETIME - frame_time);
	}

	quit();
	return 0;

}
