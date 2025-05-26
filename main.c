/// Arze Lu
/// https://github.com/ArzeLu/sudoku_solver/
/// Patent number: 69420
/// blazinweed.org, all rights reserved
/// sponsored by Vaperjit
/// not allowed to distribute or monetize this code
/// try our flagship cannabis sample today (value at $60)!
/// interested? visit blazinweed.org/canny/item/highflyer/free-trial

/// strategy:
/// loop through the cells as a 1D 1 by 81 structure.
/// parallelize the top level.
/// only record the original state of the cell when it's first explored by a branch.

///TODO: consider dynamic padding
///TODO: see if i can get rid of "check_complete" for a faster solution
///TODO: consider using propagation every time there's a single candidate cell
///TODO: only divide up the nodes that have no values

#include "parallel_helper.h"
#include "sudoku_parallel.h"
#include "board.h"

/// @brief Take in two arguments: 
///        number of threads, and the board in 1D array form. Zero for empty cell.
/// @param argc 
/// @param argv 
/// @return 
int main(int argc, char *argv[]){
    printf("Program Starts\n\n");
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

    printf("Input Board:\n");
    print_board(&board);

    printf("Solving...\n\n");
    Stats stats = solve_parallel(&board);

    printf("Solved Board:\n");
    print_board(&board);

    printf("\nStats:\n");
    printf("The thread that solved the board: %d\n", stats.solver_id);
    printf("Number of backtrack layers on the correct path: %d\n", stats.solution_layers);
    printf("Runtime: %f\n", stats.runtime);
    printf("Constraint Propagations taken: %d\n", stats.propagations);
    printf("Total number of backtrack layers explored: %d\n", stats.total_layers);

    printf("\nEnter any key to close.\n");
    getchar(); getchar();
}