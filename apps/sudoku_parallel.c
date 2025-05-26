#include "libs.h"
#include "parallel_helper.h"
#include "record_helper.h"
#include "board.h"
#include "constants.h"

static const int row[NUM_CELLS] = ROW_POSITION;
static const int col[NUM_CELLS] = COL_POSITION;
static const int box[NUM_CELLS] = BOX_POSITION;
static const int row_cell[N][N] = ROW_TRAVERSAL;
static const int col_cell[N][N] = COL_TRAVERSAL;
static const int box_cell[N][N] = BOX_TRAVERSAL;

bool solved = false;

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
    uint16_t row_mask[N];
    uint16_t col_mask[N];
    uint16_t box_mask[N];
    
    #pragma omp parallel
    {
        get_regional_masks(board, row_mask, col_mask, box_mask);
        
        #pragma omp for schedule(static)
        for(int i = 0; i < NUM_CELLS; i++){
            if(board->cells[i].value != 0) continue;

            Cell *cell = &board->cells[i];
            cell->candidates = row_mask[row[i]];
            cell->candidates &= col_mask[col[i]];
            cell->candidates &= box_mask[box[i]];
            cell->remainder = pop_count(cell->candidates);
        }
    }
    
    board->propagations++;
    return fill_all_singles(board);
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
    if(solved)
        return false;

    if(index == -1)
        return true;

    board->total_layers++;
    board->solution_layers++;
    
    add_record(board);         // Start a record for this recursion.
    add_entry(board, index);   // Add the mrv cell in the record.

    uint16_t candidates = board->cells[index].candidates;

    while(candidates){
        int value = bit_position(candidates);
        edit_cell(board, index, value, candidates);
        
        if(forward_check(board, index)){
            update_neighbors(board, index);

            if(backtrack(board, find_mrv_cell(board)))
                return true;

            reset_board(board);
        }
        candidates ^= (1 << value);
    }

    board->solution_layers--;
    rollback(board);
    return false;
}

/**
 * @brief Solve by parallelizing the top level. 
 *        Start backtracking thread portion.
 * @param board 
 */
Stats solve_parallel(Board *board){
    while(constraint_propagation_all(board));        // Repeat propagation if a cell was filled.

    Stats stats;
    stats.propagations = -1;
    stats.runtime = -1;
    stats.solution_layers = -1;
    stats.solver_id = -1;
    stats.total_layers = -1;

    double start_time = omp_get_wtime();

    #pragma omp parallel
    {
        /// Calculate work portion for each thread.
        int threads = omp_get_num_threads();
        int id = omp_get_thread_num();
        int portion = floor(NUM_CELLS / threads);
        int start = id * portion;
        int end = (id + 1 == threads) ? NUM_CELLS : start + portion;
        
        Board copy;
        copy_board(board, &copy);                    // Each thread gets its own copy of the board.

        for(int i = start; i < end; i++){
            if(copy.cells[i].value != 0) continue;   // Only start with empty cells.
            if(backtrack(&copy, i)){                 // Only true if the board is solved. 
                #pragma omp critical
                {
                    solved = true;
                    stats.solver_id = id;
                }  
            }
        }

        /// Write in the stats before the termination of this parallel block
        if(id == stats.solver_id){
            stats.solution_layers = copy.solution_layers;
            copy_board(&copy, board);
        }
        #pragma omp atomic
        stats.total_layers += copy.total_layers;
    }

    double end_time = omp_get_wtime();
    
    stats.runtime = end_time - start_time;
    stats.propagations = board->propagations;

    return stats;
}