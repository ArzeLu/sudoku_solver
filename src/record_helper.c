#include "libs.h"
#include "structures.h"
#include "sudoku_parallel_helper.h"

static const int row[NUM_CELLS] = ROW_POSITION;
static const int col[NUM_CELLS] = COL_POSITION;
static const int box[NUM_CELLS] = BOX_POSITION;
static const int row_cell[N][N] = ROW_TRAVERSAL;
static const int col_cell[N][N] = COL_TRAVERSAL;
static const int box_cell[N][N] = BOX_TRAVERSAL;

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
void update_neighbor(Board *board, Record *record, int index, uint16_t mask){
    Cell *neighbor = &board->cells[index];

    if(!(neighbor->candidates & mask)){
        record->changed[record->top] = false;
    }else{
        record->changed[record->top] = true;
        neighbor->candidates ^= mask;
        neighbor->remainder -= 1;
    }

    record->top++;
}

/**
 * @brief Given a target cell,
 *        update the candidates of the neighbors accordingly.
 *        This also acts as local constraint propagation.
 * @param board 
 * @param index 
 */
void update_neighbors(Board *board, Record *record, int index){
    int value = board->cells[index].value;
    uint16_t mask = (1 << value);

    bool visited[NUM_CELLS];
    memset(visited, 0, NUM_CELLS * sizeof(bool));
    visited[index] = true;

    for(int i = 0; i < N; i++){
        int r_index = row_cell[row[index]][i];  // Index of a cell in the neighboring row
        int c_index = col_cell[col[index]][i];  // Index of a cell in the neighboring column
        int b_index = box_cell[box[index]][i];  // Index of a cell in the neighboring box

        if(!visited[r_index]) update_neighbor(board, record, r_index, mask);
        if(!visited[c_index]) update_neighbor(board, record, c_index, mask);
        if(!visited[b_index]) update_neighbor(board, record, b_index, mask);

        visited[r_index] = true;
        visited[c_index] = true;
        visited[b_index] = true;
    }
}

void reset_neighbor(Board *board, Record *record, int index, uint16_t mask){
    if(record->changed[record->top]){
        board->cells[index].candidates ^= mask;
        board->cells[index].remainder++;
        record->top--;
    }
}

/**
 * @brief Undo the updates of the neighbors in the backtrack layers,
 *        except for the last cell.
 * @param board 
 */
void reset_neighbors(Board *board, Record *record, int index){  
    int value = board->cells[index].value;
    
    bool visited[NUM_CELLS];
    memset(visited, 0, NUM_CELLS * sizeof(bool));
    visited[index] = true;

    for(int i = 0; i < N; i++){
        int r_index = row_cell[row[index]][i];  // Index of a cell in the neighboring row
        int c_index = col_cell[col[index]][i];  // Index of a cell in the neighboring column
        int b_index = box_cell[box[index]][i];  // Index of a cell in the neighboring box

        int top = record->top;

        if(!visited[r_index] && (record->changed[record->top])){
            board->cells[r_index].candidates ^= (1 << value);
            board->cells[r_index].remainder++;
            record->top--;
        }
        if(!visited[c_index] && (record->changed[record->top])){
            board->cells[c_index].candidates ^= (1 << value);
            board->cells[c_index].remainder++;
            record->top--;
        }
        if(!visited[b_index] && (record->changed[record->top])){
            board->cells[b_index].candidates ^= (1 << value);
            board->cells[b_index].remainder++;
            record->top--;
        }

        visited[r_index] = true;
        visited[c_index] = true;
        visited[b_index] = true;
    }
}
