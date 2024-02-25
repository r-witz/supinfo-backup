#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

void printSudoku(int sudoku[9][9]);
int isNumberInRow(int sudoku[9][9], int row, int number);
int isNumberInColumn(int sudoku[9][9], int column, int number);
void generateRandomSudoku(int sudoku[9][9]);


int main() {
    // Sudoku
    srand(time(NULL)); // Initialisation du générateur de nombres aléatoires

    int sudoku[9][9] = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0}
    };

    generateRandomSudoku(sudoku);
    printSudoku(sudoku);

    return 0;
}

void printSudoku(int sudoku[9][9]) {
    write(1, "\n", 1);
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            printf("%d ", sudoku[i][j]);
        }
        write(1, "\n", 1);
    }
    write(1, "\n", 1);
}

int isNumberInRow(int sudoku[9][9], int row, int number) {
    for (int i = 0; i < 9; i++) {
        if (sudoku[row][i] == number) {
            return 1; 
        }
    }
    return 0;
}

int isNumberInColumn(int sudoku[9][9], int column, int number) {
    for (int i = 0; i < 9; i++) {
        if (sudoku[i][column] == number) {
            return 1;
        }
    }
    return 0; 
}

void generateRandomSudoku(int sudoku[9][9]) {
    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 9; j++) {
            int num;
            do {
                num = rand() % 9 + 1;
            } while (isNumberInRow(sudoku, i, num) || isNumberInColumn(sudoku, j, num));
            sudoku[i][j] = num;
        }
    }
}

