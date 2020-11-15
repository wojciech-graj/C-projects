#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdbool.h>

#define TILES_ON_SCREEN 8
#define SCROLL_BORDER 2
#define NUM_TEXTURES 1
#define NUM_OBJECTS 2

enum texture_names{T_GOAL};
enum coordinates{X, Y, Z};
enum tile_directions{L, T, R, B, D};
enum default_objects{ID_PLAYER_MARBLE, ID_GOAL};

#endif
