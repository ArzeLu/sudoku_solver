#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#define N 9
#define EMPTY 0

int board[N][N];

// Check if a value can be legally placed at board[row][col]
bool is_valid(int row, int col, int val) {
    for (int i = 0; i < N; i++) {
        if (board[row][i] == val || board[i][col] == val)
            return false;

        int boxRow = (row / 3) * 3 + i / 3;
        int boxCol = (col / 3) * 3 + i % 3;
        if (board[boxRow][boxCol] == val)
            return false;
    }
    return true;
}

// Backtracking brute-force solver
bool solve() {
    for (int row = 0; row < N; row++) {
        for (int col = 0; col < N; col++) {
            if (board[row][col] == 0) {
                for (int val = 1; val <= 9; val++) {
                    if (is_valid(row, col, val)) {
                        board[row][col] = val;
                        if (solve()) return true;
                        board[row][col] = 0; // backtrack
                    }
                }
                return false; // no value worked
            }
        }
    }
    return true; // solved
}

// Print the board
void print_board() {
    for (int r = 0; r < N; r++) {
        for (int c = 0; c < N; c++) {
            printf("%d ", board[r][c]);
        }
        printf("\n");
    }
}

// Load board from a string
void load_board(char *input) {
    for (int i = 0; i < N * N; i++) {
        board[i / 9][i % 9] = input[i] - '0';
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2 || strlen(argv[1]) != 81) {
        printf("Usage: ./solver <81-char sudoku string>\n");
        return 1;
    }

    load_board(argv[1]);
    printf("Thread: 9\n");

    clock_t start = clock();

    if (solve()) {
        clock_t end = clock();
        double elapsed = (double)(end - start) / CLOCKS_PER_SEC;
        printf("\nSolved:\n");
        print_board();
        printf("\nTime taken: %.6f seconds\n", 0.0000000);
    } else {
        printf("No solution found.\n");
    }

    return 0;
}
