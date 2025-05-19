#include "libs.h"
#include "board.h"

static const int row[NUM_CELLS] = ROW_POSITION;
static const int col[NUM_CELLS] = COL_POSITION;
static const int box[NUM_CELLS] = BOX_POSITION;
static const int row_cell[N][N] = ROW_TRAVERSAL;
static const int col_cell[N][N] = COL_TRAVERSAL;
static const int box_cell[N][N] = BOX_TRAVERSAL;

Record* generate_dummy(){
    Record *dummy = calloc(1, sizeof(Record));  // zeroes all fields
    if (!dummy) {
        printf("malloc failed in generate_dummy\n");
        exit(1);
    }
    return dummy;
}

/// @brief Given the board and the index, make a record
///        of the cell and add it to the linked list in the board.
/// @param board 
/// @param index 
void record_cell(Board *board, int index){
    Cell *cell = &board->cells[index];
    Record *new_record = malloc(sizeof(Record));
    
    if(!new_record){
        printf("error 1 in push_record");
        exit(1);
    }
    
    new_record->index = index;
    new_record->value = cell->value;
    new_record->candidates = cell->candidates;
    new_record->remainder = cell->remainder;
    new_record->next = board->record;
    board->record = new_record;
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

        update_neighbor(board, row_neighbor_index, value, visited);
        update_neighbor(board, col_neighbor_index, value, visited);
        update_neighbor(board, box_neighbor_index, value, visited);
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

        restore_neighbor(board, row_neighbor_index, value, visited);
        restore_neighbor(board, col_neighbor_index, value, visited);
        restore_neighbor(board, box_neighbor_index, value, visited);
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
