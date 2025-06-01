#include "structures.h"

/**
 * @brief Print the board with formatting
 * @param board
 */
void print_board(Board *board){
    int counter = 0;
    for(int i = 0; i < NUM_CELLS; i++){
        printf("%d  ", board->cells[i].value);
        
        counter++;

        if(!(counter % 3) && (counter % 9)){
            printf("| ");
        }
        if(!(counter % 9)){
            printf("\n");
            printf("         |          |\n");
        }
        if(!(counter % 27) && (counter != 81)){
            printf("-------------------------------\n");
        }
    }
    printf("\n\n");
}

/**
 * @brief Populate the board with given inputs.
 *        Give all struct fields initial values.
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
    board->propagations = 0;
    board->total_layers = 0;
    board->solution_layers = 0;
}

/**
 * @brief Deep copy a board, except for the records.
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
    copy->propagations = original->propagations;
    copy->total_layers = original->total_layers;
    copy->solution_layers = original->solution_layers;
}
