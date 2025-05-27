#ifndef BOARD_HELPER_H
#define BOARD_HELPER_H

#include "board.h"

void print_board(Board *board);
void populate(Board *board, char input[]);
void copy_board(Board *original, Board *copy);

#endif