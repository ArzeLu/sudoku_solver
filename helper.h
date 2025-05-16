#ifndef HELPER_H
#define HELPER_H

#include "board.h"

bool check_complete(Board *board);
void print_board(Board *board);
void populate(Board *board, char input[]);
void copy_board(Board *original, Board *copy);
void fill_single(Board *board, int index);
bool fill_all_singles(Board *board);
void update_cell(Board *board, int index, int value);
int find_mrv_cell(Board *board);
uint16_t scan_row(Board *board, int position);
uint16_t scan_col(Board *board, int position);
uint16_t scan_box(Board *board, int position);
bool scan_neighbor(Board *board, int index, int value);
int bit_position(uint16_t mask);

#endif