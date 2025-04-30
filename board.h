#include <libs.h>

#define N 9

typedef struct{
    int value;
    uint16_t candidates;
    int remaining;
} Cell;

typedef struct{
    Cell grid[N][N];
} Board;