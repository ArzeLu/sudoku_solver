/// Arze Lu
/// https://github.com/ArzeLu/sudoku_solver/

#include "parallel_helper.h"
#include "sudoku_parallel.h"
#include "board.h"

/// Take in two arguments: 
/// number of threads, and the board in 1D array form. Zero for empty cell.
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