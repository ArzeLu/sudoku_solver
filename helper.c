#include <libs.h>
#include <board.h>

static const int row[NUM_CELLS] = ROW_POSITION;
static const int col[NUM_CELLS] = COL_POSITION;
static const int box[NUM_CELLS] = BOX_POSITION;
static const int row_cell[N][N] = ROW_TRAVERSAL;
static const int col_cell[N][N] = COL_TRAVERSAL;
static const int box_cell[N][N] = BOX_TRAVERSAL;

/// Populate the board with the given inputs.
/// Not random.
void populate(Board *board, char input[]){
    for(int i = 0; i < N * N; i++){
            int value = input[i] - '0';
            board->cells[i].value = value;

            if(value > 0){
                board->cells[i].candidates = 0;
                board->cells[i].remainder = 0;
            }else{
                board->cells[i].candidates = 0x1FF;
                board->cells[i].remainder = 9;
            }
    }
    board->record = NULL;
}

/// Fill any block in the board that has just one remaining candidate
/// Return true if a value was filled
bool fill_all_singles(Board *board){
    bool filled = false;

    #pragma omp parallel for schedule(static) reduction(|:filled) if(!use_parallel) // prevent race condition of writing to the same boolean
    for(int i = 0; i < NUM_CELLS; i++){
        Cell *cell = &board->cells[i];
        if(cell->remainder == 1){
            cell->value = bit_position(cell->candidates);
            cell->remainder = 0;
            cell->candidates = 0;
            filled = true;
        }
    }
    
    return filled;
}

/// @brief Given the index, fill a single-candidate cell.
/// @param board 
/// @param index 
/// @return 
void fill_single(Board *board, int index){
    Cell *cell = &board->cells[index];

    if(cell->remainder != 1){
        printf("error 1 in fill_single");
        exit(1);
    }

    cell->value = bit_position(cell->candidates);
    cell->candidates = 0;
    cell->remainder = 0;
}

/// Deep copy board
void copy_board(Board *original, Board *copy){
    #pragma omp for schedule(static)
    for(int i = 0; i < NUM_CELLS; i++){
        copy->cells[i].candidates = original->cells[i].candidates;
        copy->cells[i].remainder = original->cells[i].remainder;
        copy->cells[i].value = original->cells[i].value;
    }
}

/// Given the position of the row,
/// Return the bitmask of available candidates of the row.
uint16_t scan_row(Board *board, int position){
    uint16_t mask = 0;
    int start = position * N;
    int end = start + N;
    for(int i = start; i < end; i++){
        int value = board->cells[i].value;
        if(value == 0) continue;
        mask |= (1 << value);
    }
    return mask;
}

/// Given the position of the column,
/// return the bitmask of available candidates of the column.
uint16_t scan_col(Board *board, int position){
    uint16_t mask = 0;
    for(int i = 0; i < N; i++){
        int value = board->cells[i * N + position].value;
        if(value == 0) continue;
        mask |= (1 << value);
    }
    return mask;
}

/// Given the position of the box,
/// return the bitmask of available candidates. 
/// The box traversal goes right then down.
uint16_t scan_box(Board *board, int position){
    uint16_t mask = 0;
    int box_row = (position / n) * n;
    int box_column = (position % n) * n;
    for(int i = 0; i < n; i++){
        for(int j = 0; j < n; j++){
            int box_x = box_row + i;
            int box_y = box_column + j;
            int index = box_x * N + box_y;
            int value = board->cells[index].value;
            if(value == 0) continue;
            mask |= (1 << value);
        }
    }

    return mask;
}

/// Given the index of the board,
/// see if the cell creates a dead end in its neighboring region.
/// (row, column, and box).
/// Returns true if it's safe.
bool scan_neighbor(Board *board, int index){
    int value = board->cells[index].value;
    uint16_t mask = (1 << value);

    for(int i = 0; i < N; i++){
        int row_neighbor_index = row_cell[row[index]][i];
        int col_neighbor_index = col_cell[col[index]][i];
        int box_neighbor_index = box_cell[box[index]][i];

        if((row_neighbor_index != index) && (board->cells[row_neighbor_index].value == 0)){
            if((board->cells[row_neighbor_index].candidates & mask) && (board->cells[row_neighbor_index].remainder == 1)) return false;
        }
        if((col_neighbor_index != index) && (board->cells[col_neighbor_index].value == 0)){
            if((board->cells[col_neighbor_index].candidates & mask) && (board->cells[col_neighbor_index].remainder == 1)) return false;
        }
        if((box_neighbor_index != index) && (board->cells[box_neighbor_index].value == 0)){
            if((board->cells[box_neighbor_index].candidates & mask) && (board->cells[box_neighbor_index].remainder == 1)) return false;
        }
    }

    return true;
}

/// @brief See if the board is solved
/// @param board 
/// @return 
bool check_complete(Board *board){
    for(int i = 0; i < NUM_CELLS; i++){
        if(board->cells[i].value == 0) return false;
    }
    return true;
}

/// Find a cell with the least remainder.
int find_mrv_cell(Board *board){
    uint8_t smallest = board->cells[0].remainder;
    int index = 0;
    for(int i = 1; i < NUM_CELLS; i++){
        uint8_t remainder = board->cells[i].remainder;
        if(remainder == 0) continue;
        if(remainder < smallest){
            smallest = remainder;
            index = i;
        }
    }

    return index;
}

/// Find what position the flipped bit is in.
/// only in the case of when only one bit is 1 in the mask,
/// or if you want to find the smallest position.
int bit_position(uint16_t mask){
    int value = 1;
    if(mask == 0) printf("error 1 in bit_position");
    while(!(mask & 1)){
        mask >>= 1;
        value++;
    }
    return value;
}

/// @brief Given the board, index to work on, and value,
///        update the cell accordingly.
/// @param board 
/// @param index 
/// @param value 
void update_cell(Board *board, int index, int value){
    Cell *cell = &board->cells[index];

    if(!(cell->candidates & (1 << value))){
        printf("error 1 in update_cell");
        exit(1);
    }
    if(remainder <= 0){
        printf("error 2 in update_cell");
        exit(2);
    }

    cell->value = value;
    cell->candidates ^= (1 << value);
    cell->remainder -= 1;
}

/// @brief Given a target index, update the candidates of the neighbors on the target's new value.
/// @param board 
/// @param index 
void update_neighbor(Board *board, int index){
    int value = board->cells[index].value;
    
    bool visited[NUM_CELLS] = {false};
    visited[index] = true;

    for(int i = 0; i < N; i++){
        int row_neighbor_index = row_cell[row[index]][i];
        int col_neighbor_index = col_cell[col[index]][i];
        int box_neighbor_index = box_cell[box[index]][i];

        if(!visited[row_neighbor_index]){
            board->cells[row_neighbor_index].candidates &= ~(1 << value);
            board->cells[row_neighbor_index].remainder -= 1;
            visited[row_neighbor_index] = true;
        }
        if(!visited[col_neighbor_index]){
            board->cells[col_neighbor_index].candidates &= ~(1 << value);
            board->cells[col_neighbor_index].remainder -= 1;
            visited[col_neighbor_index] = true;
        }
        if(!visited[box_neighbor_index]){
            board->cells[box_neighbor_index].candidates &= ~(1 << value);        
            board->cells[box_neighbor_index].remainder -= 1;
            visited[box_neighbor_index] = true;
        }
    }
}

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