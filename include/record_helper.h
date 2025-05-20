#ifndef RECORD_HELPER_H
#define RECORD_HELPER_H

#include "board.h"

Record* generate_dummy();
void record_cell(Board *board, int index);
void update_neighbor(Board *board, int index, int value);
void update_neighbors(Board *board, int index);
void restore_neighbor(Board *board, int index);
void restore_neighbors(Board *board, int index);
void undo(Board *board);
void free_record(Record **record);

#endif