#include <libs.h>
#include <board.h>

#define N 9

// populate the board with the given inputs.
// not random
void populate(Board *board, char input[]){
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            board->grid[i][j].value = input[i * N + j] - '0';
            board->grid[i][j].candidates = 0;
            board->grid[i][j].remaining = 9;
        }
    }
}

// given the board and the target cell, 
// find out the existing unique digits on the row,
// then return the digits by a 16-bit mask
uint16_t scan_row(Board **board, int x, int y){

}

// given the board and the target cell, 
// find out the existing unique digits on the column,
// then return the digits by a 16-bit maskn
uint16_t scan_column(Board **board, int x, int y){

}

// given the board and the target cell, 
// find out the existing unique digits in the box,
// then return the digits by a 16-bit mask
uint16_t scan_box(Board **board, int x, int y){

}

// find a cell that has the lowest number of remaining candidates
// to prevent needing to guess a lot.
void find_next_cell(int *x, int *y){

}

// Brian Kernighan's trick
// find the number of 1's in a binary number
int pop_count(uint16_t mask){
    int count = 0;
    while(mask){
        mask &= (mask - 1);
        count++;
    }
    return count;
}

// find positions of all flipped bits.
int* all_bits_positions(uint16_t mask, int **list){
    int position = 1;
    int count = 0;
    while(mask){
        if(mask & 1){
            *(*list + count) = position;
            count++;
        }
        mask >>= 1;
        position++;
    }
}

// find what position the flipped bit is in.
// only in the case of when only one bit is 1 in the mask.
int bit_position(uint16_t mask){
    int value = 0;
    while(mask >>= 1) value++;
    return value;
}