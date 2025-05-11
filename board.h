#include <libs.h>

#define N 9

typedef struct{
    uint8_t value;       // 0-9
    uint8_t remaining;   // 0-9
    uint16_t candidates; // bitmask for valid candidates. 1 for valid
} Cell;

typedef struct{
    Cell cells[N * N];
} Board;