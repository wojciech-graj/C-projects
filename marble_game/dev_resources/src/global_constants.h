#ifndef GLOBAL_CONSTANTS_H
#define GLOBAL_CONSTANTS_H

#define TILES_ON_SCREEN 8
#define SCROLL_BORDER 2
#define NUM_TEXTURES 1

#define MIN(a, b) ((a < b) ? a : b)
#define MAX(a, b) ((a > b) ? a : b)

enum texture_names{T_GOAL};
enum coordinates{X, Y, Z};
enum tile_directions{L, T, R, B, D};

#endif
