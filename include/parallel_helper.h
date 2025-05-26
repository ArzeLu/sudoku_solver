#ifndef PARALLEL_HELPER_H
#define PARALLEL_HELPER_H

#include "board.h"

bool check_complete(Board *board);
void print_board(Board *board);
void populate(Board *board, char input[]);
void copy_board(Board *original, Board *copy);
int bit_position(uint16_t mask);
int pop_count(uint16_t mask);
void fill_single(Board *board, int index);
bool fill_all_singles(Board *board);
void edit_cell(Board *board, int index, int value, uint16_t candidates);
int find_mrv_cell(Board *board);
bool scan_neighbors(Board *board, int index);
void get_regional_masks(Board *board, uint16_t *row_mask, uint16_t *col_mask, uint16_t *box_mask);

#endif