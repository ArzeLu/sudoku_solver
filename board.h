#ifndef BOARD_H
#define BOARD_H

#include "libs.h"
#include "constants.h"

typedef struct Record{
    uint8_t index;
    uint8_t value;       // 0-9
    uint8_t remainder;   // 0-9
    uint16_t candidates; // bitmask for valid candidates. 1 for valid
    struct Record *next;
} Record;

typedef struct Cell{
    uint8_t value;       // 0-9
    uint8_t remainder;   // 0-9
    uint16_t candidates; // bitmask for valid candidates. 1 for valid
} Cell;

typedef struct Board{
    Cell cells[NUM_CELLS];
    Record *head;
    Record *tail;
} Board;

#endif