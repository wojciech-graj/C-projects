#include "inc/renderer.h"
#include "renderer_p.h"

//TODO: individual z values for each point

void calculate_tile_side(float x_m, float x_s, float z, float tile_b, float tile_s, float tile_d, int bottom_tile_index, int side, float (*level_projection)[4], bool on_edge, unsigned char *color)
{
	if(on_edge && tile_d == 0) {
		TILE_SIDE(x_m, x_s, z, tile_b, tile_s, tile_b - EDGE_HEIGHT, tile_s - EDGE_HEIGHT, color);
	} else {
		float *bottom_tile_tile = level_projection[bottom_tile_index];
		if(tile_d != 0) {
			TILE_SIDE(x_m, x_s, z, tile_b, tile_s, tile_s - tile_d/2.f, tile_b - tile_d/2.f, color);
		} else if(tile_b > bottom_tile_tile[side] || tile_s > bottom_tile_tile[T]) {
			TILE_SIDE(x_m, x_s, z, tile_b, tile_s, bottom_tile_tile[T], bottom_tile_tile[side], color);
		}
	}
}

void AREA_TILE_PROJECTION(Area *area, float *tile, float *sub_texture, float x_m, float z) {//TODO: optimize
	Sprite *sprite = area->sprite;
	int mul_flip_x = 1 - sprite->flip_x * 2;
	int mul_flip_y = 1 - sprite->flip_y * 2;
	float width = 1.f / NUM_TEXTURE_FRAMES[sprite->texture_index];
	float x_offset = sprite->frame * width;

	float tex_x[4] = {mul_flip_x * (sub_texture[X] + area->tile_side_lengths[X]) + sprite->flip_x,
		mul_flip_x * (sub_texture[X]) + sprite->flip_x,
		mul_flip_x * (sub_texture[X]) + sprite->flip_x,
		mul_flip_x * (sub_texture[X] + area->tile_side_lengths[X]) + sprite->flip_x};

	float tex_y[4] = {mul_flip_y * (sub_texture[Y]) + sprite->flip_y,
		mul_flip_y * (sub_texture[Y]) + sprite->flip_y,
		mul_flip_y * (sub_texture[Y] + area->tile_side_lengths[Y]) + sprite->flip_y,
		mul_flip_y * (sub_texture[Y] + area->tile_side_lengths[Y]) + sprite->flip_y};

	float xpos[4] = {x_m - .5f, x_m, x_m + .5f, x_m};
	int i;
	for(i = 0; i < 4; i++)
	{
		switch(sprite->rotate)
		{
			case false:
				glTexCoord2f(x_offset + tex_x[i] * width, tex_y[i]);
				break;
			case true:
				glTexCoord2f(x_offset + tex_y[i] * width, tex_x[i]);
		}
		glVertex3f(xpos[i], tile[i], z);
	}
}

void draw_sprite(Sprite *sprite, float z) //TODO: optimize
{
	float width = 1.f / NUM_TEXTURE_FRAMES[sprite->texture_index];
	float x_offset = sprite->frame * width;

	float tex_x[4] = {sprite->flip_x, sprite->flip_x, !sprite->flip_x, !sprite->flip_x};
	float tex_y[4] = {!sprite->flip_y, sprite->flip_y, sprite->flip_y, !sprite->flip_y};

	int i;
	for(i = 0; i < 4; i++)
	{
		switch(sprite->rotate)
		{
			case false:
				glTexCoord2f(x_offset + tex_x[i] * width, tex_y[i]);
				break;
			case true:
				glTexCoord2f(x_offset + tex_y[i] * width, tex_x[i]);
		}
		glVertex3f(sprite->corner_projections[i][X], -sprite->corner_projections[i][Y], z);
	}
}

void draw_marble(Marble *marble, float z)
{
	glColor3ubv(marble->color);
	glBegin(GL_POLYGON);
	float angle;
	for(angle = 0; angle < M_TAU; angle += M_TAU / NUM_CIRCLE_POINTS)
	{
		glVertex3f(marble->radius * cosf(angle) + marble->position[X],
			marble->radius * sinf(angle) + marble->radius + (marble->position[Z] - marble->position[Y])/2.f, z);
	}
	glEnd();
}

void calculate_on_screen(Context *context)
{
	int tile_position[2];
	for(tile_position[Y] = 0; tile_position[Y] < context->height; tile_position[Y]++)
	{
		int offset = tile_position[Y] % 2;
		for(tile_position[X] = 0; tile_position[X] < context->width; tile_position[X]++)
		{
			int tile_index = tile_position[Y] * context->width + tile_position[X];
			float *tile = context->projection[tile_index];
			float x_l = tile_position[X] - .5f + offset/2.f;
			float x_r = tile_position[X] + .5f + offset/2.f;
			if((ON_SCREEN_Y(tile[T], context->scroll_offset)
				|| ON_SCREEN_Y(tile[B], context->scroll_offset))
				&& (ON_SCREEN_X(x_l, context->scroll_offset)
				|| ON_SCREEN_X(x_r, context->scroll_offset))) {
				context->on_screen[tile_index] = true;
			} else {
				context->on_screen[tile_index] = false;
			}
		}
	}
}

void draw_tiles(Context *context)
{
	glBegin(GL_TRIANGLES);
	START_FOR_EACH_TILE_ON_SCREEN
	{
		float tb_avg = (tile[B] + tile[T])/2.f;
		TILE_TRIANGLE(x_m, x_l, tile[B], tile[L], tile[T], z, (1 + tb_avg - tile[L]), context->floor_colors[tile_index]); //draw left triangle
		TILE_TRIANGLE(x_m, x_r, tile[B], tile[R], tile[T], z, (1 + tile[R] - tb_avg), context->floor_colors[tile_index]); //draw right triangle
	}
	END_FOR_EACH_TILE_ON_SCREEN
	glEnd();
}

void draw_tile_sides(Context *context)
{
	glBegin(GL_QUADS);
	START_FOR_EACH_TILE_ON_SCREEN
	{
		calculate_tile_side(x_m, x_l, z,
			tile[B], tile[L],
			context->level[tile_index][D],
			((tile_position[Y] + 1) * context->width + tile_position[X] + offset - 1), R, context->projection,
			((tile_position[X] == 0 && ! offset) || tile_position[Y] == context->height - 1),
			context->left_color); //draw left side fill
		calculate_tile_side(x_m, x_r, z,
			tile[B], tile[R],
			context->level[tile_index][D],
			((tile_position[Y] + 1) * context->width + tile_position[X] + offset), L, context->projection,
			((tile_position[X] == context->width - 1 && offset) || tile_position[Y] == context->height - 1),
			context->right_color); //draw right side fill
	}
	END_FOR_EACH_TILE_ON_SCREEN
	glEnd();
}

void draw_tile_outlines(Context *context)
{
	glColor3f(1, 1, 1);
	START_FOR_EACH_TILE_ON_SCREEN
	{
		glBegin(GL_LINE_LOOP);
		TILE_OUTLINE(x_l, x_m, x_r, z, tile);
		glEnd();
	}
	END_FOR_EACH_TILE_ON_SCREEN
}

void draw_objects(Context *context)
{
	int i;
	for(i = 0; i < context->num_objects; i++)
	{
		Object object = context->objects[i];
		switch(object.common->type)
		{
			case MARBLE: ;
			Marble *marble = object.marble;
			draw_marble(marble, (marble->tile_frac_position[Y] + marble->tile_index / context->width) / context->height);
			break;
			case AREA: ;
			Area *area = object.area;
			if(context->on_screen[area->corner_tile_indexes[L]]
				|| context->on_screen[area->corner_tile_indexes[T]]
				|| context->on_screen[area->corner_tile_indexes[R]]
				|| context->on_screen[area->corner_tile_indexes[B]]) {
				START_TEXTURE(context->textures[area->sprite->texture_index]);
				int texture_position[2];
				for(texture_position[X] = 0; texture_position[X] < area->side_lengths[X]; texture_position[X]++)
				{
					for(texture_position[Y] = 0; texture_position[Y] < area->side_lengths[Y]; texture_position[Y]++) //TODO: OPTIMIZE
					{
						float area_subposition[2] = {texture_position[X] / (float) area->side_lengths[X],
							texture_position[Y] / (float) area->side_lengths[Y]};
						float position[2] = {area->corner_positions[T][X] - .5f*texture_position[X] + .5f*texture_position[Y],
							area->corner_positions[T][Y] + .5f*texture_position[X] + .5f*texture_position[Y] + .5f};
						int tile_index;
						calculate_tile(position, &tile_index, NULL, context);
						AREA_TILE_PROJECTION(area, context->projection[tile_index], area_subposition, position[X], tile_index / (float) (context->width * context->height));
					}
				}
				END_TEXTURE();
			}
			break;
			case POINT: ;
			Point *point = object.point;
			if(context->on_screen[point->tile_index]) {
				START_TEXTURE(context->textures[point->sprite->texture_index]);
				draw_sprite(point->sprite, point->z);
				END_TEXTURE();
			}
			break;
		}
	}
}

void draw_string(const float position[2], const float size[2], const char *text)
{
	int text_length = strlen(text);
	float letter_width = size[Y] / 2.f;//TODO: rework
	float corner_positions[4][2] = {{position[X], position[Y]},
		{position[X] + letter_width, position[Y]},
		{position[X] + letter_width, position[Y] + size[Y]},
		{position[X], position[Y] + size[Y]}};
	float width = 1.f / NUM_TEXTURE_FRAMES[T_TEXT];

	const float tex_x[4] = {1.f, 1.f, 0.f, 0.f};
	const float tex_y[4] = {0.f, 1.f, 1.f, 0.f};

	int i;
	for(i = 0; i < text_length; i++)
	{
		float x_offset = (text[i] - 32) * width;
		int j;
		for(j = 0; j < 4; j++)
		{
			glTexCoord2f(x_offset + tex_y[j] * width, tex_x[j]);
			glVertex3f(corner_positions[j][X], -corner_positions[j][Y], 1.f);
			corner_positions[j][X] += letter_width;
		}
	}
}

void draw_menu(Context *context, MenuContext *menu_context)
{
	glLoadIdentity();
	gluOrtho2D(0, 1, -1, 0);
	Menu menu = MENUS[menu_context->selected_menu];
	int i;
	START_TEXTURE(context->textures[T_TEXT]);
	for(i = 0; i < menu.num_buttons; i++)
	{
		MenuButton button = menu.buttons[i];
		draw_string(button.position, button.size, button.text);
	}
	for(i = 0; i < menu.num_dynamic_texts; i++)
	{
		MenuDynamicText dynamic_text = menu.dynamic_texts[i];
		draw_string(dynamic_text.position, dynamic_text.size, get_dynamic_text(context, menu_context, dynamic_text));
	}
	END_TEXTURE();
	MenuButton button = menu.buttons[menu_context->selected_button];
	glColor4f(.5f, .5f, .5f, .5f);
	glBegin(GL_QUADS); //TODO: CREATE A PROPER FUNCTION
	glVertex3f(button.position[X], -button.position[Y], 1.f);
	glVertex3f(button.position[X] + button.size[X], -button.position[Y], 1.f);
	glVertex3f(button.position[X] + button.size[X], -button.position[Y] - button.size[Y], 1.f);
	glVertex3f(button.position[X], -button.position[Y] - button.size[Y], 1.f);
	glEnd();
}

void draw_game(Context *context)
{
	glLoadIdentity();
	gluOrtho2D(context->scroll_offset[X], TILES_ON_SCREEN_X + context->scroll_offset[X],
		-context->scroll_offset[Y]/2.f, TILES_ON_SCREEN_Y - context->scroll_offset[Y]/2.f);
	draw_tiles(context);
	draw_tile_sides(context);
	draw_tile_outlines(context);
	draw_objects(context);
}
