#include "inc/object.h"
#include "object_p.h"

Object *init_objectlist(int num_objects)
{
	Object *objects = malloc(num_objects * sizeof(Object));
	int i;
	for(i = 0; i < num_objects; i++)
	{
		objects[i].common = NULL;
	}
	return objects;
}

void delete_objectlist(Object *objects, int num_objects)
{
	int i;
	for(i = 0; i < num_objects; i++)
	{
		free(objects[i].common);
	}
	free(objects);
}
