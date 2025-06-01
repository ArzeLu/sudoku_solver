#include "libs.h"
#include "constants.h"
#include "structures.h"
#include "record_helper.h"

static const int row[NUM_CELLS] = ROW_POSITION;
static const int col[NUM_CELLS] = COL_POSITION;
static const int box[NUM_CELLS] = BOX_POSITION;
static const int row_cell[N][N] = ROW_TRAVERSAL;
static const int col_cell[N][N] = COL_TRAVERSAL;
static const int box_cell[N][N] = BOX_TRAVERSAL;

/**
 * @brief Find the position of the right-most flipped bit.
 * @param mask 
 * @return 
 */
int bit_position(uint16_t mask){
    int value = 0;

    // if(mask == 0)
    //     fprintf(stderr, "error 1 in bit_position"), exit(EXIT_FAILURE);

    while(!(mask & 1)){
        mask >>= 1;     // Shifts to the right.
        value++;
    }

    return value;
}

/**
 * @brief Brian Kernighan's trick.
 *        Find numbers of all flipped bits in the mask.
 * @param mask 
 * @return 
 */
int pop_count(uint16_t mask){
    int count = 0;
    while(mask){
        mask &= (mask - 1);
        count++;
    }

    // if(count > N)
    //     fprintf(stderr, "Error 1 in pop_count"), exit(EXIT_FAILURE);

    return count;
}

/**
 * @brief Fill any single-candidate cell on the board.
 *        Return true if a value was filled.
 * @param board
 * @return 
 */
bool fill_all_singles(Board *board){
    bool filled = false;

    /// The clause "reduction(|:filled)" prevents race condition of writing to the same boolean.
    #pragma omp parallel for schedule(static) reduction(|:filled)
    for(int i = 0; i < NUM_CELLS; i++){
        Cell *cell = &board->cells[i];
        if(cell->remainder == 1){
            cell->value = bit_position(cell->candidates);
            cell->candidates = 0;
            cell->remainder = 0;
            filled = true;
        }
    }
    return filled;
}

/**
 * @brief Find a cell with the least remainders.
 *        Return -1 if there are no cells with remainder bigger than 0.
 * @param board 
 * @return 
 */
int find_mrv_cell(Board *board){
    uint8_t smallest = N + 1;
    int index = -1;

    for(int i = 0; i < NUM_CELLS; i++){
        uint8_t remainder = board->cells[i].remainder;
        if(remainder < smallest && remainder > 0){
            smallest = remainder;
            index = i;
        }
    }

    return index;
}

/**
 * @brief See if a cell value creates a dead end
 *        for its neighbors.
 *        (row, column, and box).
 *        Returns true if it's safe.
 * @param board 
 * @param index 
 * @return 
 */
bool scan_neighbors(Board *board, int index){
    int value = board->cells[index].value;
    uint16_t mask = (1 << value);

    // if(value == 0)
    //     fprintf(stderr, "Error 1 in scan_neighbors"), exit(EXIT_FAILURE);
    // if(value > N || value < 0)
    //     fprintf(stderr, "Error 2 in scan_neighbors"), exit(EXIT_FAILURE);
    
    for(int i = 0; i < N; i++){
        int r_index = row_cell[row[index]][i];  // Index of a cell in the neighboring row
        int c_index = col_cell[col[index]][i];  // Index of a cell in the neighboring col
        int b_index = box_cell[box[index]][i];  // Index of a cell in the neighboring box

        Cell *r_neighbor = &board->cells[r_index];
        Cell *c_neighbor = &board->cells[c_index];
        Cell *b_neighbor = &board->cells[b_index];

        if((r_neighbor->candidates & mask) && (r_neighbor->remainder == 1)) return false;
        if((c_neighbor->candidates & mask) && (c_neighbor->remainder == 1)) return false;
        if((b_neighbor->candidates & mask) && (b_neighbor->remainder == 1)) return false;
    }

    return true;
}

/**
 * @brief Make masks of each region. (rows, columns, boxes)
 * @param board
 * @param row_mask
 * @param col_mask
 * @param box_mask
 */
void get_regional_masks(Board *board, uint16_t *row_mask, uint16_t *col_mask, uint16_t *box_mask){
    for(int i = 0; i < N; i++){
        row_mask[i] = INITIAL_MASK;
        col_mask[i] = INITIAL_MASK;
        box_mask[i] = INITIAL_MASK;
    }

    for(int i = 0; i < NUM_CELLS; i++){
        int value = board->cells[i].value;

        row_mask[row[i]] &= ~(1 << value);
        col_mask[col[i]] &= ~(1 << value);
        box_mask[box[i]] &= ~(1 << value);
    }
}