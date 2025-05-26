#ifndef RECORD_HELPER_H
#define RECORD_HELPER_H

#include "board.h"

void add_entry(Board *board, int index);
void add_record(Board *board);
void update_neighbors(Board *board, int index);
void reset_neighbors(Board *board);
void rollback(Board *board);

#endif