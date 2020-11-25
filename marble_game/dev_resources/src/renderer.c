#include "renderer.h"

static void draw_side(float x_m, float x_s, float tile_b, float tile_s, float bottom_tile_t, float bottom_tile_s, unsigned char *color)
{
	glColor3ubv(color);
	glBegin(GL_QUADS);
	glVertex2f(x_m, tile_b);
	glVertex2f(x_s, tile_s);
	glVertex2f(x_s, bottom_tile_t);
	glVertex2f(x_m, bottom_tile_s);
	glEnd();
}

static void calculate_draw_side(float x_m, float x_s, float tile_b, float tile_s, float tile_d, int bottom_tile_index, int side, float (*level_projection)[4], bool on_edge, unsigned char *color)
{
	if(on_edge && tile_d == 0) {
		draw_side(x_m, x_s, tile_b, tile_s, tile_b - EDGE_HEIGHT, tile_s - EDGE_HEIGHT, color);
	} else {
		float *bottom_tile_tile = level_projection[bottom_tile_index];
		if(tile_d != 0) {
			draw_side(x_m, x_s, tile_b, tile_s, tile_s - tile_d/2.f, tile_b - tile_d/2.f, color);
		} else if(tile_b > bottom_tile_tile[side] || tile_s > bottom_tile_tile[T]) {
			draw_side(x_m, x_s, tile_b, tile_s, bottom_tile_tile[T], bottom_tile_tile[side], color);
		}
	}
}

static void draw_tile_triangle(float x_m, float x_s, float tile_b, float tile_s, float tile_t, float cmul, unsigned char *floor_color)
{
	glColor3ub(imin(imax(0, floor_color[0] * cmul), 255), //floor_color does not have to be converted to int to prevent overflow because of integer promotion
		imin(imax(0, floor_color[1] * cmul), 255),
		imin(imax(0, floor_color[2] * cmul), 255));
	glVertex2f(x_m, tile_b);
	glVertex2f(x_s, tile_s);
	glVertex2f(x_m, tile_t);
}

static void draw_tile_outline(float x_l, float x_m, float x_r, float *tile)
{
	glColor3f(1, 1, 1);
	glBegin(GL_LINE_LOOP);
	glVertex2f(x_l, tile[L]);
	glVertex2f(x_m, tile[T]);
	glVertex2f(x_r, tile[R]);
	glVertex2f(x_m, tile[B]);
	glEnd();
}

//TODO: optimize
static void draw_flat_area(Area *area, float *tile, float *sub_texture, float x_l, float x_m, float x_r) {
	int mul_flip_x = 1 - area->flip_x * 2;
	int mul_flip_y = 1 - area->flip_y * 2;

	float tex_x[4] = {mul_flip_x * (sub_texture[X] - area->half_tile_side_lengths[X]) + area->flip_x,
		mul_flip_x * (sub_texture[X] + area->half_tile_side_lengths[X]) + area->flip_x,
		mul_flip_x * (sub_texture[X] + area->half_tile_side_lengths[X]) + area->flip_x,
		mul_flip_x * (sub_texture[X] - area->half_tile_side_lengths[X]) + area->flip_x};

	float tex_y[4] = {mul_flip_y * (sub_texture[Y] - area->half_tile_side_lengths[Y]) + area->flip_y,
		mul_flip_y * (sub_texture[Y] - area->half_tile_side_lengths[Y]) + area->flip_y,
		mul_flip_y * (sub_texture[Y] + area->half_tile_side_lengths[Y]) + area->flip_y,
		mul_flip_y * (sub_texture[Y] + area->half_tile_side_lengths[Y]) + area->flip_y};

	float xpos[4] = {x_l, x_m, x_r, x_m};
	int i;
	for(i = 0; i < 4; i++)
	{
		switch(area->rotate)
		{
			case false:
				glTexCoord2f(tex_x[i], tex_y[i]);
				break;
			case true:
				glTexCoord2f(tex_y[i], tex_x[i]);
		}
		glVertex2f(xpos[i], tile[i]);
	}
}

static void draw_marble(Marble *marble)
{
	glColor3ubv(marble->color);
	glBegin(GL_POLYGON);
	float angle;
	for(angle = 0; angle < M_TAO; angle += M_TAO / NUM_CIRCLE_POINTS)
	{
		glVertex2f(marble->radius * (float) cos(angle) + marble->position[X],
			marble->radius * (float) sin(angle) + marble->radius + (marble->position[Z] - marble->position[Y])/2.f);
	}
	glEnd();
}

static void scroll_screen(Context *context)
{
	glLoadIdentity();
	gluOrtho2D(0 + context->scroll_offset[X], TILES_ON_SCREEN + context->scroll_offset[X],
		0 - context->scroll_offset[Y]/2.f, TILES_ON_SCREEN - context->scroll_offset[Y]/2.f);
}

void draw(SDL_Context *sdl_context, Context *context)
{
	if(context->scroll) {
		context->scroll = false;
		scroll_screen(context);
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	int tile_position[2];
	for(tile_position[Y] = 0; tile_position[Y] < context->height; tile_position[Y]++)
	{
		int offset = tile_position[Y] % 2;
		glColor3f(1, 1, offset);
		for(tile_position[X] = 0; tile_position[X] < context->width; tile_position[X]++)
		{
			int tile_index = tile_position[Y] * context->width + tile_position[X];
			float *tile = context->projection[tile_index];
			float x_l = tile_position[X] - .5f + offset/2.f;
			float x_m = tile_position[X] + offset/2.f;
			float x_r = tile_position[X] + .5f + offset/2.f;
			float tb_avg = (tile[B] + tile[T])/2.f;

			if((ON_SCREEN_Y(tile[T], context->scroll_offset)
				|| ON_SCREEN_Y(tile[B], context->scroll_offset))
				&& (ON_SCREEN_X(x_l, context->scroll_offset)
				|| ON_SCREEN_X(x_r, context->scroll_offset))) {
				glBegin(GL_TRIANGLES);
				draw_tile_triangle(x_m, x_l, tile[B], tile[L], tile[T], (1 + (tb_avg - tile[L])), context->floor_colors[tile_index]); //draw left triangle
				draw_tile_triangle(x_m, x_r, tile[B], tile[R], tile[T], (1 + (tile[R] - tb_avg)), context->floor_colors[tile_index]); //draw right triangle
				glEnd();
				calculate_draw_side(x_m, x_l,
					tile[B], tile[L],
					context->level[tile_index][D],
					((tile_position[Y] + 1) * context->width + tile_position[X] + offset - 1), R, context->projection,
					((tile_position[X] == 0 && ! offset) || tile_position[Y] == context->height - 1),
					context->left_color); //draw left side fill
				calculate_draw_side(x_m, x_r,
					tile[B], tile[R],
					context->level[tile_index][D],
					((tile_position[Y] + 1) * context->width + tile_position[X] + offset), L, context->projection,
					((tile_position[X] == context->width - 1 && offset) || tile_position[Y] == context->height - 1),
					context->right_color); //draw right side fill
				draw_tile_outline(x_l, x_m, x_r, tile);
			}

			Area *goal = context->objects[ID_GOAL].area;
			if((ON_SCREEN_X(goal->corner_positions[L][X], context->scroll_offset)
			|| ON_SCREEN_X(goal->corner_positions[R][X], context->scroll_offset))
			&& (ON_SCREEN_Y(goal->corner_projections[T], context->scroll_offset)
			|| ON_SCREEN_Y(goal->corner_projections[B], context->scroll_offset))) {//draw goal
				float position[2] = {tile_position[X] + offset/2.f, tile_position[Y]/2.f};
				float area_position[2];
				if(in_area(goal, position, area_position)) {
					glBindTexture(GL_TEXTURE_2D, context->textures[goal->texture_index]);
					glEnable(GL_TEXTURE_2D);
					glBegin(GL_QUADS);
					draw_flat_area(goal, tile, area_position, x_l, x_m, x_r);
					glEnd();
					glDisable(GL_TEXTURE_2D);
				}
			}

			if(tile_index == context->objects[ID_PLAYER_MARBLE].marble->tile_index) { //draw ball
				draw_marble(context->objects[ID_PLAYER_MARBLE].marble);
			}
		}
	}
	SDL_GL_SwapWindow(sdl_context->window);
}
