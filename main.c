#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "sudoku.h"
#include "generate.h"
#include "ga_solver.h"

int main(void) {
    srand((unsigned)time(NULL));
    const int SIZE = 9;
    const int EMPTY = 50;

    Board solution = {0}, puzzle = {0}, found = {0};
    if (!initBoard(&solution, SIZE) || !initBoard(&puzzle, SIZE) || !initBoard(&found, SIZE)) {
        fprintf(stderr, "Bład alokacji pamięci.\n");
        return 1;
    }

    generateFullBoard(&solution);
    copyBoard(&solution, &puzzle);
    removeNumbers(&puzzle, EMPTY);

    printf("Puzzle (z %d pustymi):\n", EMPTY);
    printBoard(&puzzle);

    if (solveSudokuGA(&puzzle, &found, SELECTION_TOURNAMENT, CROSSOVER_TWO_POINT, MUTATION_SWAP)) {
        printf("\nZnalezione rozwiazanie:\n");

        printBoard(&found);
    } else {
        printf("\nNie znaleziono poprawnego rozwiazania w zadanej liczbie generacji.\n");
    }

    freeBoard(&solution);
    freeBoard(&puzzle);
    freeBoard(&found);
    return 0;
}
