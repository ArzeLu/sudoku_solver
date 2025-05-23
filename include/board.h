#ifndef BOARD_H
#define BOARD_H

#include "libs.h"
#include "constants.h"

typedef struct Cell{
    uint8_t value;           // 0-9
    uint8_t remainder;       // 0-9
    uint16_t candidates;     // bitmask for valid candidates. 1 for valid
} Cell;

typedef struct Entry{        // Individual cell changes.
    uint16_t candidates;
    int index;
    struct Entry *next;
} Entry;

typedef struct Record{       // A record of all cell changes.
    bool written[NUM_CELLS];
    Entry *entries;
    struct Record *next;
} Record;

typedef struct Board{
    Cell cells[NUM_CELLS];
    Record *records;
} Board;

#endif