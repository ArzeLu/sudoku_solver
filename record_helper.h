#ifndef RECORD_HELPER_H
#define RECORD_HELPER_H

#include "board.h"

void push_record(Board *board, int index);
void update_neighbor(Board *board, int index, int value, bool *visited);
void update_neighbors(Board *board, int index);
void restore_neighbor(Board *board, int index, int value, bool *visited);
void restore_neighbors(Board *board, int index, int value);
void undo(Board *board, Record *record);
void free_record(Record *record);

#endif