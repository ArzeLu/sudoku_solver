#ifndef SUDOKU_PARALLEL_H
#define SUDOKU_PARALLEL_H

#include "board.h"

void solve_parallel(Board *board);
bool backtrack(Board *board);
bool forward_check(Board *board, int index);
bool constraint_propagation_all(Board *board);

#endif