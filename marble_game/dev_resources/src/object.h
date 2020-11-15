#ifndef OBJECT_H
#define OBJECT_H

#include <stdlib.h>

typedef struct Context Context;
typedef struct DefaultObject DefaultObject;
typedef struct Marble Marble;
typedef struct Area Area;
typedef union Object Object;

typedef struct DefaultObject {
	void (*physics_process)(Context*, Object*);
} DefaultObject;

typedef union Object {
	DefaultObject *common;
	Marble *marble;
	Area *area;
} Object;

Object *init_objectlist(int num_objects);

#endif
