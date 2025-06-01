#ifndef SUDOKU_PARALLEL_H
#define SUDOKU_PARALLEL_H

#include "structures.h"

bool constraint_propagation_all(Board *board);
bool forward_check(Board *board, int index);
bool backtrack(Board *board);
Stats solve_parallel(Board *board);

#endif