#ifndef CHECKERS_GUI_H
#define CHECKERS_GUI_H

#include <ncurses.h>
#include <locale.h>
#include <unistd.h>

#include "checkers.h"

#define BOARD_SIDELENGTH 10
#define SCREEN_WIDTH 14
#define SCREEN_HEIGHT 15

const int EXIT_POS[] = {SCREEN_WIDTH - 1, 0};
const int RESIGN_POS[] = {10, SCREEN_HEIGHT - 1};
const int CAPTURE_POS[] = {13, SCREEN_HEIGHT - 1};
const int BOARD_OFFSET[] = {3, 3};

#endif
