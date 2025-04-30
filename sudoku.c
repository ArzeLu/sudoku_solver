#include <libs.h>
#include <board.h>
#include <helper.h>

#define N 9
#define NUM_CELLS 81

// Readjust the number of remaining posible candidates for all cells.
// Assign the cell with a value if there's only one remaining.
void constraint_propagation(Board **board){
    #pragma omp parallel for collapse(2)
    for(int i = 0; i < N; i++){
        for(int j = 0; i < N; j++){
            // filter and reduce the number of candidates
            (*board)->grid[i][j].candidates |= scan_row(board, i, j);
            (*board)->grid[i][j].candidates |= scan_column(board, i, j);
            (*board)->grid[i][j].candidates |= scan_box(board, i, j);
            
            int count = pop_count((*board)->grid[i][j].candidates);
            
            // assign the value if only one candidate is left
            if(count == 1){
                (*board)->grid[i][j].value = bit_position((*board)->grid[i][j].candidates);
                (*board)->grid[i][j].candidates = 0;
                count--;
            }
            (*board)->grid[i][j].remaining = count;
        }
    }
}

// Check all candidates to see if they produce a dead end.
// return true if no dead end exists.
bool forward_check(Board **board, int x, int y){
    uint16_t candidates = (*board)->grid[x][y].candidates;
    uint16_t neighbors = 0;

    neighbors |= scan_row(board, x, y);
    neighbors |= scan_column(board, x, y);
    neighbors |= scan_box(board, x, y);

    uint16_t result = neighbors ^ candidates;

    return result == 511;  //if all 9 bits of the right hand side are 1, meaning 2^0 + 2^1 +...+ 2^8 which is 511
}

// Check all candidates to see if they produce a dead end
//int forward_check(Board **board, int x, int y){
//    int count = (*board)->grid[x][y].remaining;
//     int *checklist = malloc(count * sizeof(int));

//     all_bits_positions((*board)->grid[x][y].candidates, &checklist);

//     for(int i = 0; i < count; i++){
//         checklist[i]
//     }
// }

int* backtrack(int **board){
    int x, y;
    find_next_cell(&x, &y);
    forward_check(board, x, y);
}

void solve(int **board){
    constraint_propagation
    backtrack()
}

int main(int argc, char *argv[]){
    int threads = 9;
    Board *board;

    if(argc > 1){
        threads = atoi(argv[1]);
    }
    if(argc > 2){
        if(strlen(argv[2]) == NUM_CELLS){
            populate(&board, argv[2]);
        }else{
            printf("\nERROR!! the number of inputs for the board is not 81\n");
        }
    }

    omp_set_num_threads(threads);

    solve(&board);
}