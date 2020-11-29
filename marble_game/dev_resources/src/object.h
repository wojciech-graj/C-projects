#ifndef OBJECT_H
#define OBJECT_H

#include <stdlib.h>

typedef struct Context Context;
typedef struct DefaultObject DefaultObject;
typedef struct Marble Marble;
typedef struct Area Area;
typedef struct Point Point;
typedef struct Sprite Sprite;
typedef union Object Object;

typedef struct DefaultObject {
	void (*physics_process)(Context*, Object);
	void (*delete)(Object);
	int type;
} DefaultObject;

typedef union Object {
	DefaultObject *common;
	Marble *marble;
	Area *area;
	Point *point;
	Sprite *sprite;
} Object;

Object *init_objectlist(int num_objects);
void delete_objectlist(Object *objects, int num_objects);
void delete_object(Object object);

#endif
