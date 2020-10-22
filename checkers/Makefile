cli:
	gcc checkers-cli.c checkers.c -o checkers-cli -lm -g
cli-windows:
	x86_64-w64-mingw32-gcc checkers-cli.c checkers.c -o checkers-cli -lm -g
gui:
	gcc checkers-gui.c checkers.c -o checkers-gui -lm `sdl2-config --libs --cflags` -lSDL2_image
gui-windows:
	mingw32-gcc checkers-gui.c checkers.c -LC:\MinGW\lib -IC:\MinGW\include -mwindows -lmingw32 -lSDL2main -lSDL2 -lSDL2_image -o windows_exe\checkers-gui.exe
