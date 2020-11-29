#ifndef GAME_H
#define GAME_H

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include <GL/gl.h>
#include <GL/glu.h>
#include <SDL2/SDL.h>

#include "area.h"
#include "context.h"
#include "global.h"
#include "level.h"
#include "marble.h"
#include "object.h"
#include "renderer.h"
#include "sprite.h"
#include "texture.h"

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 640
#define FPS 30
#define FRAMETIME (1000 / FPS)

#endif
