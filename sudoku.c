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
/// Returns true if the function filled a cell with a single remainder candidate.
/// DONE
bool constraint_propagation(Board **board){
    uint16_t row_mask[N];                                 // get the available candidates of each row.
    uint16_t col_mask[N];
    uint16_t box_mask[N];

    #pragma omp for
    for(int i = 0; i < N; i++){       // looping for the number of rows, columns, and boxes.
        row_mask[i] = scan_row(board, i);                                 // get the available candidates of each row.
        col_mask[i] = scan_col(board, i);
        box_mask[i] = scan_box(board, i);
    }
    
    #pragma omp for
    for(int i = 0; i < NUM_CELLS; i++){
        int row = i % 9;
        int column = i / 9;

        Cell *cell = &(*board)->cells[i];
        if(cell->remainder == 0) continue;
        cell->candidates |= row_mask[row];                      // update the candidates
        cell->remainder = pop_count(cell->candidates); // update the remaining count

        Cell *cell = &(*board)->cells[i];    
        if(cell->remainder == 0) continue;
        cell->candidates |= col_mask[column];
        cell->remainder = pop_count(cell->candidates);

        int box_row = i % n;                                              // row position of the top left of the box
        int box_column = i / n;                                           // column position of the top left of the box
        
        for(int j = 0; j < n; j++){
            for(int k = 0; k < n; k++){
                int box_x = box_row + j;                                        // visualize it
                int box_y = box_column + k;   
                int index = box_x * N + box_y;   
                Cell *cell = &(*board)->cells[index];        
                if(cell->remainder == 0) continue;
                cell->candidates |= box_mask[position];
                cell->remainder = pop_count(cell->candidates);
            }
        }
    }
    
    return fill_singles(board);
}

/// Checks all candidates to see if they produce a dead end.
/// Return a working value, return a zero if nothing works.
bool forward_check(Board **board, int index){
    int index = find_mrv_cell(board);

    /// assume for now that the cell is valid
    /// check cell neighbor validity
    

    bool neighbor_validity = check neighbor validity

    return box_validity & neighbor_validity
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
    while(constraint_propagation(board)); // Repeat CSP if a cell was filled. Refer to CSP function comments.
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
            printf("\nERROR!! not exactly 81 inputs!\n");
        }
    }

    omp_set_num_threads(threads);

    solve(&board);
}