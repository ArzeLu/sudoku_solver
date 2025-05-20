#ifndef HELPER_H
#define HELPER_H

#include "board.h"

bool check_complete(Board *board);
void print_board(Board *board);
void populate(Board *board, char input[]);
void copy_board(Board *original, Board *copy);
void fill_single(Board *board, int index);
bool fill_all_singles(Board *board);
void edit_cell(Board *board, int index, int value);
int find_mrv_cell(Board *board);
uint16_t get_candidates_mask(Board *board, int position);
bool scan_neighbors(Board *board, int index);
int bit_position(uint16_t mask);
int pop_count(uint16_t mask);

#endif