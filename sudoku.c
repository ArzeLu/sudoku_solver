/// Patent number: 69420
/// blazinweed.org, all rights reserved
/// sponsored by Vaperjit
/// not allowed to distribute or monetize this code
/// try our flagship cannabis sample today (value at $60)!
/// interested? visit blazinweed.org/canny/item/highflyer/free-trial

/// parallelize the top level
/// avoid dividing tasks too deep

//TODO: consider dynamic padding
#include <libs.h>
#include <board.h>
#include <helper.c>

/// Readjust the number of remaining possible candidates for all cells.
/// Assign the cell with a value if there's only one remaining.
/// Returns true if the function filled a cell with a single remainder candidate.
/// DONE
bool constraint_propagation(Board *board){
    uint16_t row_mask[N];
    uint16_t col_mask[N];
    uint16_t box_mask[N];

    #pragma omp parallel
    {
        #pragma omp for schedule(static)       // prevent openmp from choosing interleaving distribution. 
        for(int i = 0; i < N; i++){            // no "parallel" key word here because it's already in one
            row_mask[i] = scan_row(board, i);  // get the available candidates of each row.
            col_mask[i] = scan_col(board, i);  // get the available candidates of each column.
            box_mask[i] = scan_box(board, i);  // get the available candidates of each of the 3x3 box.
        }
        
        #pragma omp for schedule(static)
        for(int i = 0; i < NUM_CELLS; i++){
            int row = i / N;
            int column = i % N;
            int box_index = (row / n) * n + (column / n);

            Cell *cell = &board->cells[i];
            if(cell->remainder == 0) continue;
            cell->candidates = row_mask[row] & col_mask[column] & box_mask[box_index]; // update the candidates
            cell->remainder = pop_count(cell->candidates); // update the remaining count
        }
    }
    
    return fill_singles(board); // fill up any single-candidate cells. True if filled, false if no singles.
}

/// Checks all candidates to see if they produce a dead end.
/// Return a working value, return a zero if nothing works.
    /// assume for now that the cell is valid
    /// check cell neighbor validity
bool forward_check(Board *board, int index){
    int index = find_mrv_cell(board);
    Cell *cell = &board->cells[index];

    int candidate = bit_position(cell->candidates);


    bool neighbor_validity = check neighbor validity

    return box_validity & neighbor_validity
}

int* backtrack(int *board){
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

void solve(Board *board){
    while(constraint_propagation(board)); // Repeat CSP if a cell was filled. Refer to CSP function comments.
    backtrack() //not yet implemented
}

int main(int argc, char *argv[]){
    int threads = 9;
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
}