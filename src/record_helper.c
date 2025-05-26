#include "libs.h"
#include "board.h"
#include "parallel_helper.h"

static const int row[NUM_CELLS] = ROW_POSITION;
static const int col[NUM_CELLS] = COL_POSITION;
static const int box[NUM_CELLS] = BOX_POSITION;
static const int row_cell[N][N] = ROW_TRAVERSAL;
static const int col_cell[N][N] = COL_TRAVERSAL;
static const int box_cell[N][N] = BOX_TRAVERSAL;

/**
 * @brief Add a new record entry of a cell to the board.
 * @param board 
 * @param index 
 */
void add_entry(Board *board, int index){
    // if(board->records == NULL)
    //     fprintf(stderr, "error 1 in add_entry"), exit(EXIT_FAILURE);
    
    Record *record = board->records;
    Entry *new_entry = malloc(sizeof(Entry));

    // if(!new_entry)
    //     fprintf(stderr, "error 2 in add_entry"), exit(EXIT_FAILURE);

    new_entry->index = index;
    new_entry->candidates = board->cells[index].candidates;

    if(record->entries == NULL){
        new_entry->next = NULL;
        record->entries = new_entry;
    }else{
        new_entry->next = record->entries;
        record->entries = new_entry;
    }    
}

/**
 * @brief Add a new blank record to the board.
 * @param board 
 */
void add_record(Board *board){
    Record *new_record = malloc(sizeof(Record));

    // if(!new_record)
    //     fprintf(stderr, "error 1 in add_record"), exit(EXIT_FAILURE);

    new_record->entries = NULL;

    if(board->records == NULL){
        new_record->next = NULL;
        board->records = new_record;
    }else{
        new_record->next = board->records;
        board->records = new_record;
    }
}

/**
 * @brief Update the candidates of the neighbors of a cell
 *        with its new value by flipping the corresponding
 *        bit to zero, marking it unavailable.
 *        Function is used exclusively by update_neighbor().
 * @param board 
 * @param index 
 * @param value 
 * @param visited 
 */
void update_neighbor(Board *board, int index, uint16_t mask){
    Cell *neighbor = &board->cells[index];

    if(neighbor->candidates & mask){
        add_entry(board, index);
        neighbor->candidates ^= mask;
        neighbor->remainder -= 1;
    }
}

/**
 * @brief Given a target cell,
 *        update the candidates of the neighbors accordingly.
 *        This also acts as local constraint propagation.
 * @param board 
 * @param index 
 */
void update_neighbors(Board *board, int index){
    int value = board->cells[index].value;
    uint16_t mask = (1 << value);

    for(int i = 0; i < N; i++){
        int r_index = row_cell[row[index]][i];  // Index of a cell in the neighboring row
        int c_index = col_cell[col[index]][i];  // Index of a cell in the neighboring column
        int b_index = box_cell[box[index]][i];  // Index of a cell in the neighboring box

        update_neighbor(board, r_index, mask);
        update_neighbor(board, c_index, mask);
        update_neighbor(board, b_index, mask);
    }
}

/**
 * @brief Undo the updates of the neighbors in the backtrack layers,
 *        except for the last cell.
 * @param board 
 */
void reset_neighbors(Board *board){    
    Entry **entry = &board->records->entries;
    
    while((*entry)->next != NULL){
        int index = (*entry)->index;
        Cell *cell = &board->cells[index];

        cell->value = 0;
        cell->candidates = (*entry)->candidates;
        cell->remainder = pop_count((*entry)->candidates);

        Entry *old = *entry;
        *entry = (*entry)->next;
        free(old);
    }
}

/**
 * @brief Undo all the changes of a backtack stage.
 *        Used when an entire backtrack layer fails.
 * @param board 
 */
void rollback(Board *board){
    Entry **entry = &board->records->entries;
    Cell *cell = &board->cells[(*entry)->index];

    /// Restore MRV cell in backtrack stage to its original state.
    cell->candidates = (*entry)->candidates;
    cell->remainder = pop_count((*entry)->candidates);

    free(*entry);
    board->records->entries = NULL;

    Record *old = board->records;
    board->records = board->records->next;
    free(old);
}
