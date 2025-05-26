#ifndef RECORD_HELPER_H
#define RECORD_HELPER_H

#include "board.h"

Record* generate_dummy();
void add_entry(Board *board, int index);
void add_record(Board *board);
void free_entry(Entry *entry);
void free_record(Record *record);
void update_neighbors(Board *board, int index);
void reset_board(Board *board);
void rollback(Board *board);

#endif