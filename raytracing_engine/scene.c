#include "scene.h"

#include "jsmn.h"

const char *scene_elem_tags[] = {"Camera", "Sphere", "Triangle", "Plane", "Mesh", "Light", "AmbientLight", "Epsilon"};
const char *camera_elem_tags[] = {"position", "vector_x", "vector_y"};
const char *object_elem_tags[] = {"ks", "kd", "ka", "kr", "shininess"};
const char *sphere_elem_tags[] = {"position", "radius"};
const char *triangle_elem_tags[] = {"vertex_1", "vertex_2", "vertex_3"};
const char *plane_elem_tags[] = {"position", "normal"};
const char *mesh_elem_tags[] = {"filename", "position", "rotation", "scale"};
const char *light_elem_tags[] = {"position", "intensity"};

int get_string_index(int num_elems, const char **array, char *buffer, int string_start, int string_length)
{
	int i, j;
	for(i = 0; i < num_elems; i++)
	{
		int length = strlen(array[i]);
		if(string_length == length)
		{
			for(j = 0; j < length; j++)
			{
				if(array[i][j] != buffer[string_start + j])
					goto NO_STRING_MATCH;
			}
			return i;
		}
		NO_STRING_MATCH:
		continue;
	}
	return -1;
}

int get_string_token_index(char *buffer, jsmntok_t *tokens, int token_index, int num_elems, const char **elem_tags)
{
	assert(tokens[token_index].type == JSMN_STRING);
	jsmntok_t *string_token = &tokens[token_index];
	int string_length = string_token->end - string_token->start;
	int string_index = get_string_index(num_elems, elem_tags, buffer, string_token->start, string_length);
	assert(string_index != -1);
	return string_index;
}

float get_float(char *buffer, jsmntok_t *tokens, int token_index)
{
	assert(tokens[token_index].type == JSMN_PRIMITIVE);
	jsmntok_t *float_token = &tokens[token_index];
	int string_length = float_token->end - float_token->start;
	char *first_char = &buffer[float_token->start];
	char string[string_length + 1];
	strncpy(string, first_char, string_length);
	string[string_length] = '\0';
	return atof(string);
}

void get_float_array(int num_elems, float *array, char *buffer, jsmntok_t *tokens, int token_index)
{
	assert(tokens[token_index].type == JSMN_ARRAY && tokens[token_index].size == num_elems);
	int i;
	for(i = 0; i < num_elems; i++)
	{
		token_index++;
		array[i] = get_float(buffer, tokens, token_index);
	}
}

FILE *get_file(char *buffer, jsmntok_t *tokens, int token_index)
{
	assert(tokens[token_index].type == JSMN_STRING);
	jsmntok_t *string_token = &tokens[token_index];
	int filename_length = string_token->end - string_token->start;
	char *first_char = &buffer[string_token->start];
	char filename[filename_length + 1];
	strncpy(filename, first_char, filename_length);
	filename[filename_length] = '\0';
	FILE *file = fopen(filename, "rb");
	assert(file);
	return file;
}

Camera *scene_load_camera(char *buffer, jsmntok_t *tokens, int *token_index, int resolution[2], Vec2 image_size, float focal_length)
{
	assert(tokens[*token_index].size == NUM_CAMERA_ELEMS);

	Vec3 position, vectors[2];

	(*token_index)++;
	int i;
	for(i = 0; i < NUM_CAMERA_ELEMS; i++)
	{
		int string_index = get_string_token_index(buffer, tokens, *token_index, NUM_CAMERA_ELEMS, camera_elem_tags);
		switch(string_index)
		{
			case CAMERA_POSITION:
			get_float_array(3, position, buffer, tokens, *token_index + 1);
			*token_index += 5;
			break;
			case CAMERA_VECTOR1:
			get_float_array(3, vectors[0], buffer, tokens, *token_index + 1);
			*token_index += 5;
			break;
			case CAMERA_VECTOR2:
			get_float_array(3, vectors[1], buffer, tokens, *token_index + 1);
			*token_index += 5;
			break;
			default:
			printf("Extraneous parameter in camera in scene.\n");
			exit(0);
		}
	}
	Camera *camera = malloc(sizeof(Camera));
	init_camera(camera,
		position,
		vectors,
		focal_length,
		resolution,
		image_size);
	return camera;
}

//token_index is the index of the first string
void scene_load_object_params(char *buffer, jsmntok_t *tokens, int *token_index, Vec3 ks, Vec3 kd, Vec3 ka, Vec3 kr, float *shininess)
{
	int i;
	for(i = 0; i < NUM_OBJECT_ELEMS; i++)
	{
		int string_index = get_string_token_index(buffer, tokens, *token_index, NUM_OBJECT_ELEMS, object_elem_tags);
		switch(string_index)
		{
			case OBJECT_KS:
			get_float_array(3, ks, buffer, tokens, *token_index + 1);
			*token_index += 5;
			break;
			case OBJECT_KD:
			get_float_array(3, kd, buffer, tokens, *token_index + 1);
			*token_index += 5;
			break;
			case OBJECT_KA:
			get_float_array(3, ka, buffer, tokens, *token_index + 1);
			*token_index += 5;
			break;
			case OBJECT_KR:
			get_float_array(3, kr, buffer, tokens, *token_index + 1);
			*token_index += 5;
			break;
			case OBJECT_SHININESS:
			*shininess = get_float(buffer, tokens, *token_index + 1);
			*token_index += 2;
			break;
			default:
			printf("Extraneous parameter in object in scene.\n");
			exit(0);
		}
	}
}

//NOTE: SOME OF THE FOLLOWING FUNCTIONS COULD BE MORE OPTIMIZED BECAUSE CERTAIN SHAPES ONLY HAVE Vec3 PARAMETERS, BUT FOR THE SAKE OF EXTENSIBILITY THIS HAS NOT BEEN DONE

Sphere *scene_load_sphere(char *buffer, jsmntok_t *tokens, int *token_index)
{
	assert(tokens[*token_index].size == (NUM_OBJECT_ELEMS + NUM_SPHERE_ELEMS));
	(*token_index)++;
	OBJECT_INIT_VARS_DECL;
	scene_load_object_params(buffer, tokens, token_index, ks, kd, ka, kr, &shininess);
	Vec3 position;
	float radius;
	int i;
	for(i = 0; i < NUM_SPHERE_ELEMS; i++)
	{
		int string_index = get_string_token_index(buffer, tokens, *token_index, NUM_SPHERE_ELEMS, sphere_elem_tags);
		switch(string_index)
		{
			case SPHERE_POSITION:
			get_float_array(3, position, buffer, tokens, *token_index + 1);
			*token_index += 5;
			break;
			case SPHERE_RADIUS:
			radius = get_float(buffer, tokens, *token_index + 1);
			*token_index += 2;
			break;
			default:
			printf("Extraneous parameter in sphere in scene.\n");
			exit(0);
		}
	}
	return init_sphere(
		OBJECT_INIT_VARS,
		position,
		radius);
}

Triangle *scene_load_triangle(char *buffer, jsmntok_t *tokens, int *token_index)
{
	assert(tokens[*token_index].size == (NUM_OBJECT_ELEMS + NUM_TRIANGLE_ELEMS));
	(*token_index)++;
	OBJECT_INIT_VARS_DECL;
	scene_load_object_params(buffer, tokens, token_index, ks, kd, ka, kr, &shininess);
	Vec3 vertices[3];
	int i;
	for(i = 0; i < NUM_TRIANGLE_ELEMS; i++)
	{
		int string_index = get_string_token_index(buffer, tokens, *token_index, NUM_TRIANGLE_ELEMS, triangle_elem_tags);
		switch(string_index)
		{
			case TRIANGLE_VERTEX1:
			get_float_array(3, vertices[0], buffer, tokens, *token_index + 1);
			*token_index += 5;
			break;
			case TRIANGLE_VERTEX2:
			get_float_array(3, vertices[1], buffer, tokens, *token_index + 1);
			*token_index += 5;
			break;
			case TRIANGLE_VERTEX3:
			get_float_array(3, vertices[2], buffer, tokens, *token_index + 1);
			*token_index += 5;
			break;
			default:
			printf("Extraneous parameter in triangle in scene.\n");
			exit(0);
		}
	}
	return init_triangle(
		OBJECT_INIT_VARS,
		vertices);
}

Plane *scene_load_plane(char *buffer, jsmntok_t *tokens, int *token_index)
{
	assert(tokens[*token_index].size == (NUM_OBJECT_ELEMS + NUM_PLANE_ELEMS));
	(*token_index)++;
	OBJECT_INIT_VARS_DECL;
	scene_load_object_params(buffer, tokens, token_index, ks, kd, ka, kr, &shininess);
	Vec3 position, normal;
	int i;
	for(i = 0; i < NUM_PLANE_ELEMS; i++)
	{
		int string_index = get_string_token_index(buffer, tokens, *token_index, NUM_PLANE_ELEMS, plane_elem_tags);
		switch(string_index)
		{
			case PLANE_POSITION:
			get_float_array(3, position, buffer, tokens, *token_index + 1);
			*token_index += 5;
			break;
			case PLANE_NORMAL:
			get_float_array(3, normal, buffer, tokens, *token_index + 1);
			*token_index += 5;
			break;
			default:
			printf("Extraneous parameter in plane in scene.\n");
			exit(0);
		}
	}
	return init_plane(
		OBJECT_INIT_VARS,
		position,
		normal);
}

Mesh *scene_load_mesh(char *buffer, jsmntok_t *tokens, int *token_index)
{
	assert(tokens[*token_index].size == (NUM_OBJECT_ELEMS + NUM_MESH_ELEMS));
	(*token_index)++;
	OBJECT_INIT_VARS_DECL;
	scene_load_object_params(buffer, tokens, token_index, ks, kd, ka, kr, &shininess);
	FILE *file;
	Vec3 position, rotation;
	float scale;
	int i;
	for(i = 0; i < NUM_MESH_ELEMS; i++)
	{
		int string_index = get_string_token_index(buffer, tokens, *token_index, NUM_MESH_ELEMS, mesh_elem_tags);
		switch(string_index)
		{
			case MESH_FILENAME:
			file = get_file(buffer, tokens, *token_index + 1);
			*token_index += 2;
			break;
			case MESH_POSITION:
			get_float_array(3, position, buffer, tokens, *token_index + 1);
			*token_index += 5;
			break;
			case MESH_ROTATION:
			get_float_array(3, rotation, buffer, tokens, *token_index + 1);
			*token_index += 5;
			break;
			case MESH_SCALE:
			scale = get_float(buffer, tokens, *token_index + 1);
			*token_index += 2;
			break;
			default:
			printf("Extraneous parameter in mesh in scene.\n");
			exit(0);
		}
	}
	Mesh *mesh = stl_load(
		OBJECT_INIT_VARS,
		file,
		position,
		rotation,
		scale);
	fclose(file);
	return mesh;
}

void scene_load_light(char *buffer, jsmntok_t *tokens, int *token_index, Light *light)
{
	assert(tokens[*token_index].size == NUM_LIGHT_ELEMS);
	(*token_index)++;
	Vec3 position, intensity;
	int i;
	for(i = 0; i < NUM_LIGHT_ELEMS; i++)
	{
		int string_index = get_string_token_index(buffer, tokens, *token_index, NUM_LIGHT_ELEMS, light_elem_tags);
		switch(string_index)
		{
			case LIGHT_POSITION:
			get_float_array(3, position, buffer, tokens, *token_index + 1);
			*token_index += 5;
			break;
			case LIGHT_INTENSITY:
			get_float_array(3, intensity, buffer, tokens, *token_index + 1);
			*token_index += 5;
			break;
			default:
			printf("Extraneous parameter in light in scene.\n");
			exit(0);
		}
	}
	init_light(light, position, intensity);
}

void scene_load(FILE *scene_file, Camera **camera, int *num_objects, Object **objects, int *num_lights, Light **lights, Vec3 ambient_light_intensity, int resolution[2], Vec2 image_size, float focal_length)
{
	fseek(scene_file, 0, SEEK_END);
	unsigned int length = ftell(scene_file);
	fseek(scene_file, 0, SEEK_SET);
	char *buffer = malloc(length + 1);
	assert(fread(buffer, 1, length, scene_file) == length);
	buffer[length] = '\0';

	jsmn_parser parser;
	jsmn_init(&parser);
	int num_tokens = jsmn_parse(&parser, buffer, strlen(buffer), NULL, MAX_JSON_TOKENS);
	assert(num_tokens <= MAX_JSON_TOKENS);

	jsmntok_t tokens[num_tokens];
	jsmn_init(&parser);
	assert(jsmn_parse(&parser, buffer, strlen(buffer), tokens, num_tokens) >= 0);
	assert(tokens[0].type == JSMN_OBJECT);
	int token_index = tokens[0].start + 1;

	int object_array_size = DYNAMIC_ARRAY_INCREMENT;
	int light_array_size = DYNAMIC_ARRAY_INCREMENT;

	*camera = NULL;
	*objects = malloc(object_array_size * sizeof(Object));
	*lights = malloc(light_array_size * sizeof(Light));

	while(token_index < num_tokens)
	{
		int string_index = get_string_token_index(buffer, tokens, token_index, NUM_SCENE_ELEM_TYPES, scene_elem_tags);

		token_index++;
		switch(string_index)
		{
			case CAMERA:
				*camera = scene_load_camera(buffer, tokens, &token_index, resolution, image_size, focal_length);
			break;
			case SPHERE:
				(*objects)[*num_objects].sphere = scene_load_sphere(buffer, tokens, &token_index);
			break;
			case TRIANGLE:
				(*objects)[*num_objects].triangle = scene_load_triangle(buffer, tokens, &token_index);
			break;
			case PLANE:
				(*objects)[*num_objects].plane = scene_load_plane(buffer, tokens, &token_index);
			break;
			case MESH:
				(*objects)[*num_objects].mesh = scene_load_mesh(buffer, tokens, &token_index);
			break;
			case LIGHT:
				scene_load_light(buffer, tokens, &token_index, &(*lights)[*num_lights]);
			break;
			case AMBIENTLIGHT:
				get_float_array(3, ambient_light_intensity, buffer, tokens, token_index);
				token_index += 4;
			break;
			case EPSILON:
				epsilon = get_float(buffer, tokens, token_index);
				token_index++;
			break;
			default:
			printf("Extraneous object type in scene\n");
			exit(0);
		}
		switch(string_index)
		{
			case SPHERE:
			case TRIANGLE:
			case PLANE:
			case MESH:
			(*num_objects)++;
			if(*num_objects == object_array_size) {
				object_array_size += DYNAMIC_ARRAY_INCREMENT;
				*objects = realloc(*objects, object_array_size * sizeof(Object));
				assert(*objects);
			}
			break;
			case LIGHT:
			(*num_lights)++;
			if(*num_lights == light_array_size) {
				light_array_size += DYNAMIC_ARRAY_INCREMENT;
				*lights = realloc(*lights, light_array_size * sizeof(Light));
				assert(*lights);
			}
			break;
		}
	}
	assert(camera && *num_objects > 0 && *num_lights > 0);
	*objects = realloc(*objects, *num_objects * sizeof(Object));
	assert(*objects);
	*lights = realloc(*lights, *num_lights * sizeof(Light));
	assert(*lights);
	free(buffer);
}
