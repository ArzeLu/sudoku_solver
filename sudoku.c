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
///TODO: consider using csp every time there's a single candidate cell
///TODO: only divide up the nodes that have no values

#include "libs.h"
#include "helper.h"
#include "record_helper.h"
#include "board.h"
#include "constants.h"

static const int row[NUM_CELLS] = ROW_POSITION;
static const int col[NUM_CELLS] = COL_POSITION;
static const int box[NUM_CELLS] = BOX_POSITION;
static const int row_cell[N][N] = ROW_TRAVERSAL;
static const int col_cell[N][N] = COL_TRAVERSAL;
static const int box_cell[N][N] = BOX_TRAVERSAL;

/// @brief Readjust the number of remaining possible candidates for all cells.
///        Assign the cell with a value if there's only one remaining.
///        At the end, keeps filling the single-candidate cells and stop if none found.
/// @param board 
/// @param parallelize 
/// @return 
bool constraint_propagation(Board *board, bool parallelize){
    #pragma omp parallel if(parallelize)
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

            if(cell->value != 0) continue;

            uint16_t mask = row_mask[row[i]] & col_mask[col[i]] & box_mask[box[i]];

            if(!parallelize){                  // Need to consider cell records if serialized, signaling backtrack stage.
                if(cell->candidates != mask){  // Only add a record if candidates changed.

                }
            }

            cell->candidates = mask;// update the candidates
            cell->remainder = pop_count(cell->candidates);    // update the remaining count
        }
    }
    
    return fill_all_singles(board, parallelize);
}

/// @brief Check if the cell value produces a dead end.
///        Check only the neighbors,
///        as the cell validation is guaranteed.
/// @param board 
/// @param index 
/// @param value 
/// @return 
bool forward_check(Board *board, int index){
    return scan_neighbor(board, index); // Check neighbor validity
}

/// @brief Find solution by guessing.
///        Recursively try each candidate of each cell.
///        Retreat a level if all candidates failed.
/// @param board 
/// @param visited 
/// @return 
bool backtrack(Board *board){
    if(check_complete(board))
        return true;

    /// Find mrv index, explore all candidates with that index,
    /// then roll back the recursion if all failed.
    int index = find_mrv_cell(board);
    
    add_record(board);         // Start a record for this recursion.
    add_entry(board, index);   // Add the mrv cell in the record.

    Record *current = board->record;
    uint16_t candidates = board->cells[index].candidates;

    if(board->cells[index].remainder == 1){          // Go the CSP route if one candidate remains.
        fill_single(board, index);
        while(constraint_propagation(board, false));
        if(backtrack(board))
            return true;
    }else{                                           // Otherwise, try all candidates.
        while(candidates){
            int value = bit_position(candidates);
            edit_cell(board, index, value);
            
            if(forward_check(board, index))
                if(backtrack(board))
                    return true;
                    
            restore_neighbors(board, index, value);
            candidates ^= (1 << value);
        }
    }

    undo(board, current);
    free_record(&current);
    current->next = NULL;
    return false;
}

/// @brief Solve by parallelizing the top level. 
///        Start backtracking thread portion.
/// @param board 
void solve(Board *board){
    while(constraint_propagation(board, true));            // Repeat CSP if a cell was filled. Refer to CSP function comments.

    #pragma omp parallel
    {
        /// Calculate work portion for each thread.
        int threads = omp_get_num_threads();
        int id = omp_get_thread_num();
        int portion = floor(NUM_CELLS / threads);
        int start = id * portion;
        int end = (id + 1 == threads) ? NUM_CELLS : start + portion;

        Board copy;         // Prevent shared memory faults.
        copy_board(board, &copy);                     // Each thread gets its own copy of the board.

        for(int i = start; i < end; i++){
            if(&copy.cells[i].value != 0) continue;  // Only start with empty cells.
            if(backtrack(&copy)){                     // Only true if the board is solved.
                #pragma omp critical
                {
                    copy_board(&copy, board);         // Copy the board back to the shared memory for main.
                }
            }
            // free_record(&copy->head);
            // copy->head = generate_dummy();
            // copy->tail = copy->head;
        }
        free_record(&copy.record);
        free(&copy);
    }
}

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