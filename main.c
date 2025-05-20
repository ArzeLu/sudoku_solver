
#include "board.h"
#include "helper.h"
#include "sudoku_parallel.h"

/// @brief Take in two arguments: 
///        number of threads, and the board in 1D array form. Zero for empty cell.
/// @param argc 
/// @param argv 
/// @return 
int main(int argc, char *argv[]){
    int threads = 1;
    Board board;

    if(argc > 1){
        threads = atoi(argv[1]);
    }
    if(argc > 2){
        if(strlen(argv[2]) == NUM_CELLS){
            populate(&board, argv[2]);
        }else{
            printf("\nERROR!! not exactly 81 inputs!\n");
        }
    }

    omp_set_num_threads(threads);

    solve(&board);
    print_board(&board);
}