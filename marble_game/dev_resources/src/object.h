#ifndef OBJECT_H
#define OBJECT_H

#include <stdlib.h>

#include "global.h"

typedef struct Context Context;
typedef struct DefaultObject DefaultObject;
typedef struct Marble Marble;
typedef struct Area Area;
typedef struct Point Point;
typedef struct Sprite Sprite;
typedef struct CollisionArea CollisionArea;
typedef union Object Object;

typedef struct DefaultObject {
	DEFAULT_OBJECT_PARAMS
} DefaultObject;

typedef union Object {
	DefaultObject *common;
	Marble *marble;
	Area *area;
	Point *point;
	Sprite *sprite;
	CollisionArea *collision_area;
} Object;

Object *init_objectlist(int num_objects);
void delete_objectlist(Object *objects, int num_objects);

#endif
