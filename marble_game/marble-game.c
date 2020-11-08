#include "marble-game.h"

typedef struct Marble {
	float position[3];
	int tile_index;
	float tile_position[2];
	float velocity[3];
	float radius;
	bool in_air;
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

    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	gluOrtho2D(0, TILES_ON_SCREEN, 0, TILES_ON_SCREEN);
}

void quit(void)
{
	SDL_GL_DeleteContext(main_context);
	SDL_DestroyWindow(window);
	SDL_Quit();
	free(level);
}

void draw_side(float x_m, float x_s, float tile_b, float tile_s, float bottom_tile_t, float bottom_tile_s, unsigned char *color)
{
	glColor3ubv(color);
	glBegin(GL_QUADS);
	glVertex2f(x_m, tile_b);
	glVertex2f(x_s, tile_s);
	glVertex2f(x_s, bottom_tile_t);
	glVertex2f(x_m, bottom_tile_s);
	glEnd();
}

void calculate_draw_side(float x_m, float x_s, float tile_b, float tile_s, float tile_d, int bottom_tile_index, int side, float (*level_projection)[4], bool on_edge, unsigned char *color)
{
	if(on_edge && tile_d == 0) {
		draw_side(x_m, x_s, tile_b, tile_s, tile_b - EDGE_HEIGHT, tile_s - EDGE_HEIGHT, color);
	} else {
		float *bottom_tile_tile = level_projection[bottom_tile_index];
		if(tile_d != 0) {
			draw_side(x_m, x_s, tile_b, tile_s, tile_s - tile_d/2., tile_b - tile_d/2., color);
		} else if(tile_b > bottom_tile_tile[side] || tile_s > bottom_tile_tile[t]) {
			draw_side(x_m, x_s, tile_b, tile_s, bottom_tile_tile[t], bottom_tile_tile[side], color);
		}
	}
}

void calculate_projection(float (*level_projection)[4])
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

void draw_tile_triangle(float x_m, float x_s, float tile_b, float tile_s, float tile_t, float cmul)
{
	glBegin(GL_TRIANGLES);
	glColor3ub(MIN(MAX(0, floor_color[0] * cmul), 255), //floor_color does not have to be converted to int to prevent overflow because of integer promotion
		MIN(MAX(0, floor_color[1] * cmul), 255),
		MIN(MAX(0, floor_color[2] * cmul), 255));
	glVertex2f(x_m, tile_b);
	glVertex2f(x_s, tile_s);
	glVertex2f(x_m, tile_t);
	glEnd();
}

void draw_tile_outline(float x_l, float x_m, float x_r, float *tile)
{
	glColor3f(1, 1, 1);
	glBegin(GL_LINE_LOOP);
	glVertex2f(x_l, tile[l]);
	glVertex2f(x_m, tile[t]);
	glVertex2f(x_r, tile[r]);
	glVertex2f(x_m, tile[b]);
	glEnd();
}

void draw_marble(Marble *marble)
{
	float ball_x = marble->position[x];
	float ball_y = (marble->position[z] - marble->position[y])/2.;

	glColor3fv(GREEN);
	glBegin(GL_POLYGON);
	float angle;
	for(angle = 0; angle < M_TAO; angle += M_TAO / NUM_CIRCLE_POINTS)
	{
		glVertex2f(marble->radius * cos(angle) + ball_x,
			marble->radius * sin(angle) + marble->radius + ball_y);
	}
	glEnd();
}

void draw(void)
{
	if(scroll) {
		glLoadIdentity();
		gluOrtho2D(0, TILES_ON_SCREEN, 0 - screen_scroll/2., TILES_ON_SCREEN - screen_scroll/2.);
		scroll = false;
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	float level_projection[level_height * level_width][4];
	calculate_projection(level_projection);

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

			if(ON_SCREEN(tile[t]) || ON_SCREEN(tile[b])) {
				draw_tile_triangle(x_m, x_l, tile[b], tile[l], tile[t], (1 + (tb_avg - tile[l]))); //left triangle
				calculate_draw_side(x_m, x_l,
					tile[b], tile[l],
					level[tile_index][d],
					((tile_position[y] + 1) * level_width + tile_position[x] + offset - 1), r, level_projection,
					((tile_position[x] == 0 && ! offset) || tile_position[y] == level_height - 1),
					left_color); //left side fill
				draw_tile_triangle(x_m, x_r, tile[b], tile[r], tile[t], (1 + (tile[r] - tb_avg))); // right triangle
				calculate_draw_side(x_m, x_r,
					tile[b], tile[r],
					level[tile_index][d],
					((tile_position[y] + 1) * level_width + tile_position[x] + offset), l, level_projection,
					((tile_position[x] == level_width - 1 && offset) || tile_position[y] == level_height - 1),
					right_color); //right side fill
				draw_tile_outline(x_l, x_m, x_r, tile);
			}

			if(tile_index == player_marble->tile_index) { //draw ball
				draw_marble(player_marble);
			}
		}
	}
	SDL_GL_SwapWindow(window);
}

//calculates the z value of point (posx,posy) on the plane which intersects (x1,y1,z1),(x2,y2,z2),(x3,y3,z3)
float calculate_z_on_plane(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3, float posx, float posy) {
	float v1[3] = {x2 - x1, y2 - y1, z2 - z1};
	float v2[3] = {x3 - x2, y3 - y2, z3 - z2};
	float v1xv2[3] = {v1[y]*v2[z] - v1[z]*v2[y],
		v1[z]*v2[x] - v1[x]*v2[z],
		v1[x]*v2[y] - v1[y]*v2[x]}; //cross product of v1 and v2
	float d = v1xv2[x] * x1 + v1xv2[y] * y1 + v1xv2[z] * z1;
	return (d - v1xv2[x] * posx - v1xv2[y] * posy) / v1xv2[z];
}

//calculates tile_index and tile_position
//x_pos and y_pos are coordinates from position, but with axes rotated 45 degrees anticlockwise, y shifted by .5, scaled up by sqrt(2)
//tile_index and tile_position can be easily calculated using the rotated coordinates
/* graph of tile indexes after rotation of axes:
x
2      2 5
1    1 4 8
0  0 3 7
-1   6
   0 1 2 3 y
*/
void calculate_tile(float *position, int *tile_index, float *tile_position)
{
	int posx = floor(position[x] - position[y] + .5);
	int posy = floor(position[x] + position[y] + .5);

	*tile_index = level_width * (posy - posx) + posx + floor((posy - posx)/2.);

	float offset = .5 * ((abs(posx) + abs(posy)) % 2 == 0);
	double i;
	tile_position[x] = modf(position[x] + offset, &i);
	tile_position[y] = modf(position[y] + offset, &i);
}

void physics_process_marble(Marble *marble)
{
	//calculate marble->velocity
	float *tile = level[marble->tile_index];
	float tb_avg = (tile[t] + tile[b])/2.;
	if(! marble->in_air) {
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
	} else {
		marble->velocity[z] -= GRAVITY_ACCELERATION;
	}

	float future_position[3];
	future_position[x] = marble->position[x] + marble->velocity[x];
	future_position[y] = marble->position[y] + marble->velocity[y];

	//calculate marble->tile_index and marble->tile_position
	int future_tile_index;
	float future_tile_position[2];
	calculate_tile(future_position, &future_tile_index, future_tile_position);

	float *future_tile = level[future_tile_index];
	tb_avg = (future_tile[t] + future_tile[b])/2.;

	//calculate marble->position and collision
	future_position[z] = calculate_z_on_plane(
		round(future_tile_position[x]), .5, future_tile[(future_tile_position[x] < .5) ? l : r],
		.5, 0, future_tile[t],
		.5, 1, future_tile[b],
		future_tile_position[x], future_tile_position[y]);
	if(future_position[z] - marble->position[z] > MAX_DELTA_Z) {
		marble->velocity[x] = 0;
		marble->velocity[y] = 0;
	} else {
		marble->tile_index = future_tile_index;
		marble->tile_position[x] = future_tile_position[x];
		marble->tile_position[y] = future_tile_position[y];
		marble->position[x] = future_position[x];
		marble->position[y] = future_position[y];
		if(marble->position[z] - future_position[z] > MAX_DELTA_Z) {
			if(!marble->in_air) marble->in_air = true;
		} else {
			if(marble->in_air) marble->in_air = false;
			marble->velocity[z] = future_position[z] - marble->position[z];
		}
		marble->position[z] += marble->velocity[z];
	}

	if(marble->position[y] + 1 - screen_scroll >= TILES_ON_SCREEN/2.) {
		screen_scroll = .1;
		scroll = true;
	}
}

void init_marble(Marble **marble)
{
	*marble = malloc(sizeof(Marble));
	(*marble)->position[x] = 0;
	(*marble)->position[y] = 0;
	calculate_tile((*marble)->position, &((*marble)->tile_index), (*marble)->tile_position);
	float *tile = level[(*marble)->tile_index];
	(*marble)->position[z] = (tile[t] + tile[b])/2.; //assume that x is in middle of tile
	(*marble)->velocity[x] = 0;
	(*marble)->velocity[y] = 0;
	(*marble)->radius = .2;
	(*marble)->in_air = false;
	(*marble)->physics_process = &physics_process_marble;
}

void load_level(char *filename)
{
	FILE *file = fopen(filename, "rb");
	assert(file);
	short buffer;

	fread(&level_width, sizeof(short), 1, file);
	fread(&level_height, sizeof(short), 1, file);

	level = malloc(sizeof(float) * level_height * level_width * 5);

	int i;
	for(i = 0; i < level_width * level_height; i++)
	{
		int j;
		for(j = 0; j < 5; j++)
		{
			fread(&buffer, sizeof(short), 1, file);
			level[i][j] = buffer/2.;
		}
	}
	fread(floor_color, sizeof(unsigned int), 3, file);
	fread(left_color, sizeof(unsigned int), 3, file);
	fread(right_color, sizeof(unsigned int), 3, file);

	fclose(file);
}

void input_process(void)
{
	SDL_Event event;

	while (SDL_PollEvent(&event) > 0)
	{
		if (event.type == SDL_QUIT) {
			quit();
			exit(0);
		}
	}
	if(!player_marble->in_air) {
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
}

int main(void)
{
	init_sdl();

	load_level("resources/level1");

	init_marble(&player_marble);
	assert(player_marble);

	while(true)
	{
		Uint32 frame_start = SDL_GetTicks();
		input_process();
		player_marble->physics_process(player_marble);
		draw();
		Uint32 frame_time = SDL_GetTicks() - frame_start;
		SDL_Delay(FRAMETIME - frame_time);
	}

	quit();
	return 0;
}
