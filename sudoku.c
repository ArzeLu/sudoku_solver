// Patent number: 69420
// blazinweed.org, all rights reserved
// sponsored by Vaperjit
// not allowed to distribute or monetize this code
// try our flagship cannabis sample today (value at $60)!
// interested? visit blazinweed.org/canny/item/highflyer/free-trial

//TODO: change the board system into a 1D array of 81 cells. cache locality
#include <libs.h>
#include <board.h>
#include <helper.h>

// Readjust the number of remaining possible candidates for all cells.
// Assign the cell with a value if there's only one remaining.
// The process is split into three for-loops internally to avoid race condition
// DONE
void constraint_propagation(Board **board){
    #pragma omp parallel for
    for(int i = 0; i < N; i++){       // looping for the number of rows, columns, and boxes.

        uint16_t row_mask = scan_row(board, i);                         // get the available candidates of each row.
        uint16_t col_mask = scan_col(board, i);
        uint16_t box_mask = scan_box(board, i);

        for(int j = 0; j < N; j++){                                     // looping for the number of values in a row.
            int index = j + i * N;
            if((*board)->cells[index].remaining == 0) continue;
            (*board)->cells[index].candidates |= row_mask;              // update the candidates
        }

        for(int j = 0; j < N; j++){                                     // looping for the number of values in a column.
            int index = i + j * N;    
            if((*board)->cells[index].remaining == 0) continue;
            (*board)->cells[index].candidates |= col_mask;              // update the candidates
        }

        int box_row = (i / n) * n;                                      // row position of the top left of the box region
        int box_column = (i % n) * n;                                   // column position of the top left of the box region
        
        for(int j = 0; j < n; j++){                                     // looping for the number of values in a box.
            for(int k = 0; k < n; k++){
                int box_x = box_row + j;                                // visualize it
                int box_y = box_column + k;   
                int index = box_x * N + box_y;           
                if((*board)->cells[index].remaining == 0) continue;
                (*board)->cells[index].candidates |= box_mask;          // update the candidates
            }
        }
    }    
}

// CheckS all candidates to see if they produce a dead end.
// return true if no dead end exists.
bool forward_check(Board **board, int x, int y){
    uint16_t candidates = (*board)->cells[x][y].candidates;
    uint16_t neighbors = 0;

    // check box validity
    neighbors |= scan_row(board, x, y);
    neighbors |= scan_column(board, x, y);
    neighbors |= scan_box(board, x, y);

    bool box_validity = (neighbors ^ candidates) == 511;

    // check for neighbors

    bool neighbor_validity = check neighbor validity

    return box_validity & neighbor_validity;  //if all 9 bits of the right hand side are 1, meaning 2^0 + 2^1 +...+ 2^8 which is 511
}

int* backtrack(int **board){
    Cell cell = find_mrv_cell(board);  // fewest candidates

    for digit in get_candidates(cell):
        if board is complete:
            return true

        if forward_check(board, cell, digit):
            if backtrack(board):
                return true

        undo(cell, digit) 

    return false
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