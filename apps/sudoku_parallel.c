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

/**
 * @brief Readjust the number of remaining possible candidates for all cells.
 *        Assign the cell with a value if there's only one remaining.
 *        At the end, keeps filling the single-candidate cells and stop if none found.
 *        Broad propagation for the whole board, used before backtracking.
 * @param board 
 * @param parallelize 
 * @return 
 */
bool constraint_propagation_all(Board *board){
    #pragma omp parallel
    {
        uint16_t regional_mask[N];

        #pragma omp for schedule(static)       // Prevent openmp from choosing interleaving distribution. 
        for(int i = 0; i < N; i++){            // No "parallel" key word here because it's already in one.
            regional_mask[i] = get_candidaets_mask(board, i);  // Get the available candidates of each row.
        }
        
        #pragma omp for schedule(static)
        for(int i = 0; i < NUM_CELLS; i++){
            if(board->cells[i].value != 0) continue;        // No need to update already populated cells.

            Cell *cell = &board->cells[i];
            uint16_t mask = regional_mask[i];

            cell->candidates = mask;                        // Update the candidates.
            cell->remainder = pop_count(cell->candidates);  // Update the remaining count.
        }
    }
    
    return fill_all_singles(board);
}

/**
* @brief Constraint propagation but only for adjusting the candidates of
*        the neighbors aftering filling in a single cell value.
*        This function is used during backtracking.
*        Takes in the index and the board, and use the target cell value 
*        to update the candidates of its neighbors.
* @param board 
*/
void constraint_propagation_single(Board *board, int index){
    int value = board->cells[index].value;

    for(int i = 0; i < N; i++){
        int r_index = row_cell[row[index]][i];  // Index of a cell in the neighboring row
        int c_index = col_cell[col[index]][i];
        int b_index = box_cell[box[index]][i];

        update_neighbor(board, r_index, value);
        update_neighbor(board, c_index, value);
        update_neighbor(board, b_index, value);
    }
}

/**
* @brief Check if the cell value produces a dead end.
*        Check only the neighbors,
*        as the cell validation is guaranteed.
* @param board 
* @param index 
* @param value 
* @return boolean, if the new cell value is valid.
*/
bool forward_check(Board *board, int index){
    return scan_neighbors(board, index); // Check neighbor validity
}

/**
 * @brief Find solution by guessing.
 *         Recursively try each candidate of each cell.
 *         Retreat a level if all candidates failed.
 * @param board 
 * @param visited 
 * @return 
 */
bool backtrack(Board *board, int index){
    if(index == -1)
        return true;
    
    add_record(board);         // Start a record for this recursion.
    add_entry(board, index);   // Add the mrv cell in the record.

    if(board->cells[index].remainder == 1){          // Go the propagation route if one candidate remains.
        fill_single(board, index);                   // Fill up the single-candidate cell,
        constraint_propagation_single(board, false); // then update the candidate masks and check for more singles.

        if(backtrack(board, find_mrv_cell(board)))
            return true;

    }else{                                           // Otherwise, try all candidates.
        uint16_t candidates = board->cells[index].candidates;

        while(candidates){
            int value = bit_position(candidates);
            edit_cell(board, index, value);
            
            if(forward_check(board, index)){
                update_neighbors(board, index);

                if(backtrack(board, find_mrv_cell(board)))
                    return true;

                restore_neighbors(board, index);
            }

            candidates ^= (1 << value);
        }
    }

    undo(board);
    free_record(board);

    return false;
}

/**
 * @brief Solve by parallelizing the top level. 
 *        Start backtracking thread portion.
 * @param board 
 */
void solve_parallel(Board *board){
    while(constraint_propagation_all(board));         // Repeat propagation if a cell was filled.

    #pragma omp parallel
    {
        /// Calculate work portion for each thread.
        int threads = omp_get_num_threads();
        int id = omp_get_thread_num();
        int portion = floor(NUM_CELLS / threads);
        int start = id * portion;
        int end = (id + 1 == threads) ? NUM_CELLS : start + portion;

        Board copy;
        copy_board(board, &copy);                     // Each thread gets its own copy of the board.

        for(int i = start; i < end; i++){
            if(&copy.cells[i].value != 0) continue;   // Only start with empty cells.
            if(backtrack(&copy, i)){                     // Only true if the board is solved.
                #pragma omp critical
                {
                    copy_board(&copy, board);         // Copy the board back to the shared memory for main.
                }
            }
        }
        free_record(copy.record);
    }
}