/// Patent number: 69420
/// blazinweed.org, all rights reserved
/// sponsored by Vaperjit
/// not allowed to distribute or monetize this code
/// try our flagship cannabis sample today (value at $60)!
/// interested? visit blazinweed.org/canny/item/highflyer/free-trial

/// parallelize the top level
/// avoid dividing tasks too deep
/// strategy: divide up the top level 9, then use the spare ones to start backtracking from another starting cell.

//TODO: change the board system into a 1D array of 81 cells. cache locality
#include <libs.h>
#include <board.h>
#include <helper.c>

/// Readjust the number of remaining possible candidates for all cells.
/// Assign the cell with a value if there's only one remaining.
/// The process is split into three for-loops internally to avoid race condition
/// Returns if there is a cell with just a single candidate remaining.
/// DONE
bool constraint_propagation(Board **board){
    bool singles = false;

    #pragma omp parallel for
    for(int i = 0; i < N; i++){       // looping for the number of rows, columns, and boxes.

        uint16_t row_mask = scan_row(board, i);                                 // get the available candidates of each row.
        uint16_t col_mask = scan_col(board, i);
        uint16_t box_mask = scan_box(board, i);

        for(int j = 0; j < N; j++){                                             // looping for the number of values in a row.
            int index = j + i * N;
            Cell *cell = &(*board)->cells[index];
            if(cell->remainder == 0) continue;
            cell->candidates |= row_mask;                      // update the candidates
            cell->remainder = pop_count(cell->candidates); // update the remaining count
        }

        for(int j = 0; j < N; j++){
            int index = i + j * N;
            Cell *cell = &(*board)->cells[index];    
            if(cell->remainder == 0) continue;
            cell->candidates |= col_mask;
            cell->remainder = pop_count(cell->candidates);
        }

        int box_row = (i / n) * n;                                              // row position of the top left of the box region
        int box_column = (i % n) * n;                                           // column position of the top left of the box region
        
        for(int j = 0; j < n; j++){
            for(int k = 0; k < n; k++){
                int box_x = box_row + j;                                        // visualize it
                int box_y = box_column + k;   
                int index = box_x * N + box_y;   
                Cell *cell = &(*board)->cells[index];        
                if(cell->remainder == 0) continue;
                cell->candidates |= box_mask;
                cell->remainder = pop_count(cell->candidates);
                if(cell->remainder == 1) singles = true;
            }
        }
    }  
    
    return singles;
}

/// Checks all candidates to see if they produce a dead end.
/// Return a working value, return a zero if nothing works.
bool forward_check(Board **board, int index){
    int index = find_mrv_cell(board);

    /// assume for now that the cell is valid
    /// check cell neighbor validity

    bool neighbor_validity = check neighbor validity

    return box_validity & neighbor_validity;  //if all 9 bits of the right hand side are 1, meaning 2^0 + 2^1 +...+ 2^8 which is 511
}

int* backtrack(int **board){
    int index = find_mrv_cell(board);  // fewest candidates

    for digit in get_candidates(cell):
        if board is complete:
            return true

        if forward_check(board, cell, digit):
            if backtrack(board):
                return true

        undo(cell, digit) 

    return false
}

void solve(Board **board){
    while(constraint_propagation(board)){
        fill_singles(board);
    }
    backtrack() //not yet implemented
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