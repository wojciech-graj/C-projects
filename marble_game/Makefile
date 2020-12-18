GAME_FILES := main.c area.c context.c texture.c level.c marble.c renderer.c object.c global.c sprite.c point.c collision_area.c menu.c
LINUX_CFLAGS := -lm `sdl2-config --libs --cflags` -lGL -lGLU -std=c11
WINDOWS_CFLAGS := -LC:\MinGW\lib -IC:\MinGW\include -mwindows -lmingw32 -lSDL2main -lSDL2 -lopengl32 -lglu32
RELEASE_CFLAGS := -O3
DEBUG_CFLAGS := -Wall -Wextra -Wdouble-promotion -Wpedantic -Wstrict-prototypes -Wshadow -g -fsanitize=address -fsanitize=undefined -Og -DDEBUG

GAME_PATH = dev_resources/src/
TOOLS_PATH = dev_resources/tools/

debug:
	gcc $(addprefix $(GAME_PATH), $(GAME_FILES)) -o game $(LINUX_CFLAGS) $(DEBUG_CFLAGS)
release:
	gcc $(addprefix $(GAME_PATH), $(GAME_FILES)) -o game $(LINUX_CFLAGS) $(RELEASE_CFLAGS)
level_serializer:
	gcc $(addprefix $(TOOLS_PATH), level_serializer.c) -o $(addprefix $(TOOLS_PATH), level_serializer)
texture_serializer:
	gcc $(addprefix $(TOOLS_PATH), texture_serializer.c) -o $(addprefix $(TOOLS_PATH), texture_serializer)
windows:
	mingw32-gcc $(addprefix $(GAME_PATH), $(GAME_FILES)) -o game_win.exe $(WINDOWS_CFLAGS) $(RELEASE_CFLAGS)
