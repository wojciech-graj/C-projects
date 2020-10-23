#ifndef CHECKERS_CLI_H
#define CHECKERS_CLI_H

#include "checkers.h"

static const char *PLAYER_COLORS[] = {"White", "Black"};

const char *TITLE ="\
 ___                     _   _\n\
|   \\ _ _ __ _ _  _ __ _| |_| |_ ___\n\
| |) | '_/ _` | || / _` | ' \\  _(_-<\n\
|___/|_| \\__,_|\\_,_\\__, |_||_\\__/__/\n\
                   |___/ ";

const char *HELP = "\
+---------+--------------------------------------------------------------+\n\
| COMMAND | ACTION                                                       |\n\
+---------+--------------------------------------------------------------+\n\
| resign  | resign game                                                  |\n\
+---------+--------------------------------------------------------------+\n\
| capture | execute forced sequence of captures if there exists only one |\n\
|         | possible sequence                                            |\n\
+---------+--------------------------------------------------------------+\n\
| help    | display this page                                            |\n\
+---------+--------------------------------------------------------------+\n\
| A-B     | move a piece from A to B                                     |\n\
+---------+--------------------------------------------------------------+\n\
| AxB     | capture using piece A which will land on tile B.             |\n\
|         | can be extended to perform a sequence of captures e.g. AxBxC |\n\
+---------+--------------------------------------------------------------+\n";

void end_game(int color);
static void print_board(int *board);
static void play_player_move(int color, int *board);

#endif
