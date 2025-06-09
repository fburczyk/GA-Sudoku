#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "sudoku.h"
#include "generate.h"
#include "ga_solver.h"

int main(void) {
    srand((unsigned)time(NULL));
    const int SIZE = 9;
    const int EMPTY = 40;  // liczba usuniętych pól

    Board solution = {0}, puzzle = {0}, found = {0};
    if (!initBoard(&solution, SIZE) || !initBoard(&puzzle, SIZE) || !initBoard(&found, SIZE)) {
        fprintf(stderr, "Błąd alokacji pamięci.\n");
        return 1;
    }

    // Generuj pełną planszę i usuń liczby
    generateFullBoard(&solution);
    copyBoard(&solution, &puzzle);
    removeNumbers(&puzzle, EMPTY);

    printf("Puzzle (z %d pustymi):\n", EMPTY);
    printBoard(&puzzle);

    // Rozwiąż algorytmem genetycznym
    if (solveSudokuGA(&puzzle, &found)) {
        printf("\nZnalezione rozwiązanie:\n");
        printBoard(&found);
    } else {
        printf("\nNie znaleziono rozwiązania w zadanej liczbie generacji.\n");
    }

    freeBoard(&solution);
    freeBoard(&puzzle);
    freeBoard(&found);
    return 0;
}
