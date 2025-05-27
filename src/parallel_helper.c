#include "libs.h"
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
 * @brief Fill the single-candidate target cell.
 * @param board 
 * @param index 
 * @return 
 */
void fill_single(Board *board, int index){
    Cell *cell = &board->cells[index];

    // if(cell->remainder > 1)
    //     fprintf(stderr, "error 1 in fill_single"), exit(EXIT_FAILURE);

    // if(cell->remainder < 1)
    //     fprintf(stderr, "error 2 in fill_single"), exit(EXIT_FAILURE);

    cell->value = bit_position(cell->candidates);
    cell->candidates = 0;
    cell->remainder = 0;
}

/**
 * @brief Fill any single-candidate cell on the board.
 *        Return true if a value was filled.
 * @param board
 * @return 
 */
bool fill_all_singles(Board *board){
    int filled = 0;

    /// The clause "reduction(|:filled)" prevents race condition of writing to the same boolean.
    #pragma omp parallel for schedule(static) reduction(|:filled)
    for(int i = 0; i < NUM_CELLS; i++){
        if(board->cells[i].remainder == 1){
            fill_single(board, i);
            filled = 1;
        }
    }
    return filled;
}

/**
 * @brief Assign a new value to a cell.
 *        Assign zero to its candidates and remainder.
 * @param board 
 * @param index 
 * @param value 
 * @param candidates
 */
void edit_cell(Board *board, int index, int value, uint16_t candidates){
    Cell *cell = &board->cells[index];

    // if(!(candidates & (1 << value)))
    //     fprintf(stderr, "error 1 in update_cell"), exit(EXIT_FAILURE);

    // if(pop_count(candidates) <= 0)
    //     fprintf(stderr, "error 2 in update_cell"), exit(EXIT_FAILURE);

    cell->value = value;
    cell->candidates = 0;
    cell->remainder = 0;
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
 * @brief See if the neighbor has a certain value,
 *        given by a mask.
 *        This function is exclusively created for scan_neighbors.
 * @param board
 * @param neighbor_index
 * @param mask
 */
bool scan_neighbor(Board *board, int neighbor_index, uint16_t mask){
    Cell *neighbor = &board->cells[neighbor_index];
    return (neighbor->candidates ^ mask) || (neighbor->remainder != 1);
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

        if(!scan_neighbor(board, r_index, mask)) return false;
        if(!scan_neighbor(board, c_index, mask)) return false;
        if(!scan_neighbor(board, b_index, mask)) return false;
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