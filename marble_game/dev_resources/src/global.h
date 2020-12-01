#ifndef GLOBAL_H
#define GLOBAL_H

#define TILES_ON_SCREEN 8
#define SCROLL_BORDER 2
#define NUM_TEXTURES 2
#define NUM_OBJECTS 8

#ifdef DEBUG
#define DBG_LOG(str, val) printf(str, val)
#else
#define DBG_LOG(str, val)
#endif

#define DEFAULT_OBJECT_PARAMS void (*physics_process)(Context*, Object);\
	int type;

enum texture_names{T_GOAL, T_FLAG_RED};
enum object_types{MARBLE, AREA, POINT, SPRITE, COLLISIONAREA};
enum coordinates{X, Y, Z};
enum tile_directions{L, T, R, B, D};
enum default_objects{ID_PLAYER_MARBLE, ID_GOAL};

extern int num_texture_frames[NUM_TEXTURES];

#endif
