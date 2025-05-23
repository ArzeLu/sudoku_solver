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
 * @brief Given the board and the index, make a record
 *        of the cell and add it to the linked list in the board.
 * @param board 
 * @param index 
 */
void add_entry(Board *board, int index){
    if(board->records == NULL)
        fprintf(stderr, "error 1 in add_entry"), exit(EXIT_FAILURE);

    if(board->records->written[index])
        return;
    
    Record *record = board->records;
    Entry *new_entry = malloc(sizeof(Entry));

    if(!new_entry)
        fprintf(stderr, "error 2 in add_entry"), exit(EXIT_FAILURE);

    record->written[index] = true;
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

void add_record(Board *board){
    Record *new_record = malloc(sizeof(Record));

    new_record->entries = NULL;
    memset(new_record->written, 0, sizeof(new_record->written));

    if(board->records == NULL){
        new_record->next = NULL;
        board->records = new_record;
    }else{
        new_record->next = board->records;
        board->records = new_record;
    }
}

void free_entry(Entry *entry){
    entry->next = NULL;
    free(entry);
}

void free_record(Record *record){    
    record->next = NULL;
    free(record);
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
 * @brief Given a target index, update the candidates of the neighbors on the target's new value.
 *        Also acts as local region constraint propagation.
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

void revert_neighbors(Board *board){    
    Entry *entry = board->records->entries;
    
    while(entry->next != NULL){
        board->cells[entry->index].candidates = entry->candidates;
        board->cells[entry->index].remainder = pop_count(entry->candidates);

        Entry *old = entry;
        entry = entry->next;
        free_entry(old);
    }
}

void rollback(Board *board, int value){
    Entry *entry = board->records->entries;
    Cell *cell = &board->cells[entry->index];

    /// Restore MRV cell in backtrack stage to its original state.
    cell->value = value;
    cell->candidates = entry->candidates;
    cell->remainder = pop_count(entry->candidates);

    free_entry(entry);

    Record *old = board->records;
    board->records = board->records->next;
    free_record(old);
}
