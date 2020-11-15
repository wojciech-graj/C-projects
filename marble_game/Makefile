GAME_FILES := game.c area.c context.c texture.c level.c marble.c math_functions.c renderer.c object.c
GAME_PATH = dev_resources/src/
GAME_CFLAGS := -lm `sdl2-config --libs --cflags` -lGL -lGLU -std=c11
RELEASE_CFLAGS := -O3
DEBUG_CFLAGS := -Wall -Wextra -Wdouble-promotion -Wpedantic -Wstrict-prototypes -Wshadow -g -fsanitize=address -fsanitize=undefined -Og

TOOLS_PATH = dev_resources/tools/

debug:
	gcc $(addprefix $(GAME_PATH), $(GAME_FILES)) -o game $(GAME_CFLAGS) $(DEBUG_CFLAGS)
release:
	gcc $(addprefix $(GAME_PATH), $(GAME_FILES)) -o game $(GAME_CFLAGS) $(RELEASE_CFLAGS)
level_serializer:
	gcc $(addprefix $(TOOLS_PATH), level_serializer.c) -o $(addprefix $(TOOLS_PATH), level_serializer)
texture_serializer:
	gcc $(addprefix $(TOOLS_PATH), texture_serializer.c) -o $(addprefix $(TOOLS_PATH), texture_serializer)
