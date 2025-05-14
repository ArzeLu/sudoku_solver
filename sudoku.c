/// Arze Lu
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

#include <helper.c>

/// Readjust the number of remaining possible candidates for all cells.
/// Assign the cell with a value if there's only one remaining.
/// At the end, keeps filling the single-candidate cells and stop if none found.
/// DONE
bool constraint_propagation(Board *board){
    #pragma omp parallel if(!use_parallel)
    {
        uint16_t row_mask[N];
        uint16_t col_mask[N];
        uint16_t box_mask[N];

        #pragma omp for schedule(static)       // prevent openmp from choosing interleaving distribution. 
        for(int i = 0; i < N; i++){            // no "parallel" key word here because it's already in one
            row_mask[i] = scan_row(board, i);  // get the available candidates of each row.
            col_mask[i] = scan_col(board, i);  // get the available candidates of each column.
            box_mask[i] = scan_box(board, i);  // get the available candidates of each of the 3x3 box.
        }
        
        #pragma omp for schedule(static)
        for(int i = 0; i < NUM_CELLS; i++){
            int row = i / N;                     // position of the row given cell index
            int col = i % N;                     // position of the col given cell index
            int box = (row / n) * n + (col / n); // position of the box given cell index

            Cell *cell = &board->cells[i];
            if(cell->remainder == 0) continue;
            cell->candidates = row_mask[row] & col_mask[col] & box_mask[box]; // update the candidates
            cell->remainder = pop_count(cell->candidates); // update the remaining count
        }
    }
    
    return fill_all_singles(board);
}

/// Checks all candidates to see if they produce a dead end.
/// Return a working value, return a zero if nothing works.
bool forward_check(Board *board, int index){
    Cell *cell = &board->cells[index];
    record_cell(board, index);

    uint16_t candidates = cell->candidates;

    while(candidates){                               // While candidates is not zero,
        int candidate = bit_position(candidates);    // Get the value of one candidate from the bitmask.
        candidates ^= (1 << candidate);              // Clear the target candidate out of the pool of candidates.
        update_cell(board, index, candidate);        // Update the cell with the next candidate value.
        if(scan_neighbor(board, index)) return true; // Return true when a candidate is valid.
    }
    
    return false;
}

bool backtrack(Board *board, bool *visited){
    if(check_complete(board))
        return true;
        
    int index = find_mrv_cell(board); // Return the index of the cell with least number of remainders.

    if(!visited[index])
        push_record(board, index);

    if(board->cells[index].remainder == 0){
        
    }else{

    }

    bool validity = forward_check(board, index);

    if(!validity)
        undo(board);

    return backtrack(board, visited);
}

void solve(Board *board){
    while(constraint_propagation(board)); // Repeat CSP if a cell was filled. Refer to CSP function comments.

    #pragma omp parallel
    {
        int threads = omp_get_num_threads();
        int id = omp_get_thread_num();
        int portion = floor(NUM_CELLS / threads);
        int start;
        int end;

        if(id + 1 == threads){
            start = NUM_CELLS - (id - 1) * portion;
            end = NUM_CELLS - 1;
        }else{
            start = id * portion;
            end = start + portion;
        }

        Board copy;
        copy_board(board, &copy);

        for(int i = start; i < end; i++){
            bool visited[NUM_CELLS] = {false};
            backtrack(&copy, &visited);
        }
    }
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