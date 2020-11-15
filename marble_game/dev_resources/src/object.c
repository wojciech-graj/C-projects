#include "object.h"

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
