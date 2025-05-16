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
///TODO: hardcode the fucking traversals. it's fixed sized board anyway.
///TODO: make helpers for restore and update

#include "libs.h"
#include "board.h"
#include "constants.h"

static const int row[NUM_CELLS] = ROW_POSITION;
static const int col[NUM_CELLS] = COL_POSITION;
static const int box[NUM_CELLS] = BOX_POSITION;
static const int row_cell[N][N] = ROW_TRAVERSAL;
static const int col_cell[N][N] = COL_TRAVERSAL;
static const int box_cell[N][N] = BOX_TRAVERSAL;

atomic_int solved = 0;

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
            Cell *cell = &board->cells[i];
            if(cell->remainder == 0) continue;
            cell->candidates = row_mask[row[i]] & col_mask[col[i]] & box_mask[box[i]]; // update the candidates
            cell->remainder = pop_count(cell->candidates); // update the remaining count
        }
    }
    
    return fill_all_singles(board);
}

/// Checks to see if a cell produce a dead end.
bool forward_check(Board *board, int index, int value){
    Cell *cell = &board->cells[index];

    return scan_neighbor(board, index, value);
}

/// @brief Find a
/// @param board 
/// @param visited 
/// @return 
bool backtrack(Board *board){
    if(atomic_load(&solved))
        return false;

    if(check_complete(board))
        return true;

    Record *current = board->record;

    /// Find mrv index, explore all candidates with that index,
    /// then roll back the recursion once this cell index is used up.
    int index = find_mrv_cell(board); // Return the index of the cell with least number of remainders.
    push_record(board, index);

    uint16_t candidates = board->cells[index].candidates;

    while(candidates){
        int value = bit_position(candidates);
        update_cell(board, index, value);

        if(forward_check(board, index, value))
            if(backtrack(board))
                return true;
                
        restore_neighbors(board, index, value);
        candidates ^= (1 << value);
    }

    undo(board, current);
    free_record(current);
    return false;
}

void solve(Board *board){
    while(constraint_propagation(board)); // Repeat CSP if a cell was filled. Refer to CSP function comments.

    #pragma omp parallel
    {
        int threads = omp_get_num_threads();
        int id = omp_get_thread_num();
        int portion = floor(NUM_CELLS / threads);
        int start = id * portion;
        int end = (id + 1 == threads) ? NUM_CELLS : start + portion;

        Board copy;
        copy_board(board, &copy);

        Record *current = copy.record;

        for(int i = start; i < end; i++){
            if(backtrack(&copy)){
                atomic_store(&solved, 1);
                print_board(&copy);
            }
            if(atomic_load(&solved))
                break;
        }
        free_record(current);
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