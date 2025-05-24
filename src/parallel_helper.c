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
 * @brief See if the board is solved.
 * @param board 
 * @return 
 */
bool check_complete(Board *board){
    for(int i = 0; i < NUM_CELLS; i++){
        if(board->cells[i].value == 0) return false;
    }
    return true;
}

void print_board(Board *board){
    int counter = 0;
    for(int i = 0; i < NUM_CELLS; i++){
        printf("%d  ", board->cells[i].value);
        counter++;
        if(counter == 9){
            printf("\n");
            counter = 0;
        }
    }
    printf("\n\n");
}

/**
 * @brief Populate the board with given inputs.
 * @param board 
 * @param input 
 */
void populate(Board *board, char input[]){    
    for(int i = 0; i < NUM_CELLS; i++){
            Cell *cell = &board->cells[i];

            int value = input[i] - '0'; // Convert char to int.
            cell->value = value;

            if(value > 0){
                cell->candidates = 0;
                cell->remainder = 0;
            }else{
                cell->candidates = INITIAL_MASK;
                cell->remainder = 9;
            }
    }
    board->records = NULL;
}

/**
 * @brief Deep copy a board.
 * @param original 
 * @param copy 
 */
void copy_board(Board *original, Board *copy){
    for(int i = 0; i < NUM_CELLS; i++){
        Cell *new_cell = &copy->cells[i];
        Cell *old_cell = &original->cells[i];

        new_cell->candidates = old_cell->candidates;
        new_cell->remainder = old_cell->remainder;
        new_cell->value = old_cell->value;
    }
    copy->records = NULL;
}

/**
 * @brief Find the position of the right-most flipped bit.
 * @param mask 
 * @return 
 */
int bit_position(uint16_t mask){
    int value = 0;

    if(mask == 0)
        fprintf(stderr, "error 1 in bit_position"), exit(EXIT_FAILURE);

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

    if(count > N)
        fprintf(stderr, "Error 1 in pop_count"), exit(EXIT_FAILURE);

    return count;
}

/**
 * @brief Given the index of a single-candidate cell, fill it.
 * @param board 
 * @param index 
 * @return 
 */
void fill_single(Board *board, int index){
    Cell *cell = &board->cells[index];

    if(cell->remainder > 1)
        fprintf(stderr, "error 1 in fill_single"), exit(EXIT_FAILURE);

    if(cell->remainder < 1)
        fprintf(stderr, "error 2 in fill_single"), exit(EXIT_FAILURE);

    cell->value = bit_position(cell->candidates);
    cell->candidates = 0;
    cell->remainder = 0;
}

/**
 * @brief Fill any block in the board that has just one remaining candidate.
 *        Return true if a value was filled.
 * @param board 
 * @param parallel 
 * @return 
 */
bool fill_all_singles(Board *board){
    bool filled = false;

    /// The clause "reduction(|:filled)" prevents race condition of writing to the same boolean.
    #pragma omp parallel for schedule(static) reduction(|:filled)
    for(int i = 0; i < NUM_CELLS; i++){
        if(board->cells[i].remainder == 1){
            fill_single(board, i);
            filled = true;
        }
    }
    return filled;
}

/**
 * @brief Edit a target cell given its index and a new value.
 * @param board 
 * @param index 
 * @param value 
 */
void edit_cell(Board *board, int index, int value){
    Cell *cell = &board->cells[index];

    if(!(cell->candidates & (1 << value))) // Cell value isn't available in its candidates
        fprintf(stderr, "error 1 in update_cell"), exit(EXIT_FAILURE);

    if(cell->remainder <= 0)
        fprintf(stderr, "error 2 in update_cell"), exit(EXIT_FAILURE);

    cell->value = value;
    cell->candidates ^= (1 << value);
    cell->remainder -= 1;
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

bool scan_neighbor(Board *board, int neighbor_index, uint16_t mask){
    Cell *neighbor = &board->cells[neighbor_index];

    if(neighbor->value == 0)
        if(neighbor->candidates & mask)
            if(neighbor->remainder == 1)
                return false;
            
    return true;
}

/**
 * @brief Given the index of the board,
 *        see if the cell creates a dead end in its neighboring region.
 *        (row, column, and box).
 *        Returns true if it's safe.
 * @param board 
 * @param index 
 * @param value 
 * @return 
 */
bool scan_neighbors(Board *board, int index){
    int value = board->cells[index].value;
    uint16_t mask = (1 << value);

    if(value == 0)
        fprintf(stderr, "Error 1 in scan_neighbors"), exit(EXIT_FAILURE);

    if(value > N || value < 0)
        fprintf(stderr, "Error 2 in scan_neighbors"), exit(EXIT_FAILURE);
    
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

void get_row_masks(Board *board, uint16_t row_mask[]){
    /// When a value in the region is found,
    /// turn the corresponding mask bit to zero.
    /// Output isn't affected if the value is zero.
    #pragma omp for schedule(static)
    for(int i = 0; i < N; i++){
        row_mask[i] = INITIAL_MASK;
        for(int j = 0; j < N; j++){
            int r_index = row_cell[i][j];
            row_mask[i] &= ~(1 << board->cells[r_index].value);
        }
    }
}

void get_col_masks(Board *board, uint16_t col_mask[]){
    #pragma omp for schedule(static)
    for(int i = 0; i < N; i++){
        col_mask[i] = INITIAL_MASK;
        for(int j = 0; j < N; j++){
            int c_index = col_cell[i][j];
            col_mask[i] &= ~(1 << board->cells[c_index].value);
        }
    }
}

void get_box_masks(Board *board, uint16_t box_mask[]){
    #pragma omp for schedule(static)
    for(int i = 0; i < N; i++){
        box_mask[i] = INITIAL_MASK;
        for(int j = 0; j < N; j++){
            int b_index = box_cell[i][j];
            int value = board->cells[b_index].value;
            box_mask[i] &= ~(1 << value);
        }
    }
}

void get_regional_masks(Board *board, uint16_t *row_mask, uint16_t *col_mask, uint16_t *box_mask){
    get_row_masks(board, row_mask);
    get_col_masks(board, col_mask);
    get_box_masks(board, box_mask);
}
