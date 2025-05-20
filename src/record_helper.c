#include "libs.h"
#include "board.h"

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
void record_cell(Board *board, int index){
    Cell *cell = &board->cells[index];
    Record *new_record = malloc(sizeof(Record));
    
    if(!new_record){  // malloc fail catch
        printf("error 1 in push_record");
        exit(1);
    }
    
    new_record->index = index;
    new_record->value = cell->value;
    new_record->candidates = cell->candidates;
    new_record->remainder = cell->remainder;
    
    if(board->record == NULL){
        new_record->next = NULL;
        board->record = new_record;
    }else{
        new_record->next = board->record;
        board->record = new_record;
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
void update_neighbor(Board *board, int index, int value){
    Cell *neighbor = &board->cells[index];

    if(!board->record->written[index]){
        add_entry(board, index);
        if(neighbor->candidates & (1 << value)){
            neighbor->candidates ^= (1 << value);
            neighbor->remainder -= 1;
        }
    }
}

/**
 * @brief Given a target index, update the candidates of the neighbors on the target's new value.
 * @param board 
 * @param index 
 */
void update_neighbors(Board *board, int index){
    int value = board->cells[index].value;
    
    bool visited[NUM_CELLS] = {false};          // Avoid clashes. Row, column, and box regions have overlapping cells.
    visited[index] = true;

    for(int i = 0; i < N; i++){
        int r_index = row_cell[row[index]][i];  // Index of a cell in the neighboring row
        int c_index = col_cell[col[index]][i];  // Index of a cell in the neighboring column
        int b_index = box_cell[box[index]][i];  // Index of a cell in the neighboring box

        update_neighbor(board, r_index, value, visited);
        update_neighbor(board, c_index, value, visited);
        update_neighbor(board, b_index, value, visited);
    }
}

/**
 * @brief Restore the neighbors by resetting their candidate masks.
 *        Function is used exclusively by restore_neighbors().
 * @param board 
 * @param index 
 * @param value 
 * @param visited 
 */
void restore_neighbor(Board *board, int index, int value, bool *visited){
    Cell *cell = &board->cells[index];

    if(!visited[index]){   // Avoid clashes. Row, column, and box regions have overlapping cells.
        if(!(cell->candidates & (1 << value))){
            cell->candidates |= (1 << value);
            cell->remainder += 1;
        }
        visited[index] = true;
    }
}

void restore_neighbors(Board *board, int index, int value){    
    bool visited[NUM_CELLS] = {false};
    visited[index] = true;
    
    for(int i = 0; i < N; i++){                 // Avoid clashes. Row, column, and box regions have overlapping cells.
        int r_index = row_cell[row[index]][i];  // Index of a cell in the neighboring row
        int c_index = col_cell[col[index]][i];  // Index of a cell in the neighboring column
        int b_index = box_cell[box[index]][i];  // Index of a cell in the neighboring box

        restore_neighbor(board, r_index, value, visited);
        restore_neighbor(board, c_index, value, visited);
        restore_neighbor(board, b_index, value, visited);
    }
}

void undo(Board *board, Record *record){
    if(record == NULL) return;

    undo(board, record->next);

    int index = record->index;
    Cell *cell = &board->cells[index];

    cell->candidates = record->candidates;
    cell->value = record->value;
    cell->remainder = record->remainder;
}

void free_record(Record **record){
    if(*record == NULL) return;

    free_record(&((*record)->next));
    
    (*record)->next = NULL;
    free(*record);
    (*record) = NULL;

    return;
}
