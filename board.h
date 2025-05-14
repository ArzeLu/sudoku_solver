#include <libs.h>

#define n 3
#define N 9
#define NUM_CELLS 81

typedef struct{
    uint8_t value;       // 0-9
    uint8_t remainder;   // 0-9
    uint16_t candidates; // bitmask for valid candidates. 1 for valid
} Cell;

typedef struct{
    Cell cells[NUM_CELLS];
    Record *record;
} Board;

typedef struct{
    uint8_t index;
    uint8_t value;       // 0-9
    uint8_t remainder;   // 0-9
    uint16_t candidates; // bitmask for valid candidates. 1 for valid
    Record *prev;
    Record *next;
} Record;