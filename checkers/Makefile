cli:
	gcc checkers-cli.c checkers.c -o checkers-cli -lm -g -O3
cli-windows:
	x86_64-w64-mingw32-gcc checkers-cli.c checkers.c -o checkers-cli -lm -g -O3
gui:
	gcc checkers-gui.c checkers.c -o checkers-gui -lm `sdl2-config --libs --cflags` -lSDL2_image -O3
gui-windows:
	mingw32-gcc checkers-gui.c checkers.c -LC:\MinGW\lib -IC:\MinGW\include -mwindows -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -O3 -o windows_exe\checkers-gui.exe
