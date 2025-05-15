#include "libs.h"
#include "board.h"

static const int row[NUM_CELLS] = ROW_POSITION;
static const int col[NUM_CELLS] = COL_POSITION;
static const int box[NUM_CELLS] = BOX_POSITION;
static const int row_cell[N][N] = ROW_TRAVERSAL;
static const int col_cell[N][N] = COL_TRAVERSAL;
static const int box_cell[N][N] = BOX_TRAVERSAL;

/// @brief Given the board and the index, make a record
///        of the cell and add it to the linked list in the board.
/// @param board 
/// @param index 
void push_record(Board *board, int index){
    Cell *cell = &board->cells[index];
    Record *new = malloc(sizeof(Record));
    if(!new) printf("error 1 in push_record");
    
    new->index = index;
    new->value = cell->value;
    new->candidates = cell->candidates;
    new->remainder = cell->remainder;

    if(board->record == NULL){
        board->record = new;
        board->record->prev = NULL;
        board->record->next = NULL;
    }else{
        board->record->next = new;
        new->prev = board->record;
        new->next = NULL;
        board->record = new;
    }
}

void update_neighbor(Board *board, int index, int value, bool *visited){
    if(!visited[index]){
        if(board->cells[index].candidates & (1 << value)){
            board->cells[index].candidates &= ~(1 << value);
            board->cells[index].remainder -= 1;
        }
        visited[index] = true;
    }
}

/// @brief Given a target index, update the candidates of the neighbors on the target's new value.
/// @param board 
/// @param index 
void update_neighbors(Board *board, int index){
    int value = board->cells[index].value;
    
    bool visited[NUM_CELLS] = {false};
    visited[index] = true;

    for(int i = 0; i < N; i++){
        int row_neighbor_index = row_cell[row[index]][i];
        int col_neighbor_index = col_cell[col[index]][i];
        int box_neighbor_index = box_cell[box[index]][i];

        update_neighbor(board, row_neighbor_index, value, &visited);
        update_neighbor(board, col_neighbor_index, value, &visited);
        update_neighbor(board, box_neighbor_index, value, &visited);
    }
}

void restore_neighbor(Board *board, int index, int value, bool *visited){
    if(!visited[index]){
        if(!(board->cells[index].candidates & (1 << value))){
            board->cells[index].candidates |= (1 << value);
            board->cells[index].remainder += 1;
        }
        visited[index] = true;
    }
}

void restore_neighbors(Board *board, int index, int value){    
    bool visited[NUM_CELLS] = {false};
    visited[index] = true;
    
    for(int i = 0; i < N; i++){
        int row_neighbor_index = row_cell[row[index]][i];
        int col_neighbor_index = col_cell[col[index]][i];
        int box_neighbor_index = box_cell[box[index]][i];

        restore_neighbor(board, row_neighbor_index, value, &visited);
        restore_neighbor(board, col_neighbor_index, value, &visited);
        restore_neighbor(board, box_neighbor_index, value, &visited);
    }
}

void undo(Board *board, Record *record){
    int index = record->index;
    Cell *cell = &board->cells[index];
    int value = board->cells[index].value;

    restore_neighbors(board, index, value);

    cell->candidates = record->candidates;
    cell->value = record->value;
    cell->remainder = record->remainder;
}

void free_record(Record *record){
    if(record == NULL) return;

    free_record(record->next);

    record->next = NULL;
    record->prev = NULL;

    free(record);

    return;
}
