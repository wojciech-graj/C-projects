#ifndef GAME_H
#define GAME_H

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include "global_constants.h"
#include "context.h"
#include "marble.h"
#include "level.h"
#include "area.h"
#include "renderer.h"
#include "texture.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 640
#define FPS 30
#define FRAMETIME (1000 / FPS)

#endif
