#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "sudoku.h"

bool initBoard(Board *board, int size) {
    board->size = size;
    board->boxSize = (int)sqrt(size);
    board->values = malloc(size * sizeof(int *));
    if (!board->values) return false;
    for (int i = 0; i < size; i++) {
        board->values[i] = malloc(size * sizeof(int));
        if (!board->values[i]) {
            for (int j = 0; j < i; j++) free(board->values[j]);
            free(board->values);
            board->values = NULL;
            return false;
        }
        for (int j = 0; j < size; j++)
            board->values[i][j] = 0;
    }
    return true;
}

void freeBoard(Board *board) {
    if (!board->values) return;
    for (int i = 0; i < board->size; i++)
        free(board->values[i]);
    free(board->values);
    board->values = NULL;
}

void printBoard(Board *board) {
    int n = board->size;
    int b = board->boxSize;
    int lineLength = n * 3 + (n / b - 1) * 2;

    for (int i = 0; i < n; i++) {
        if (i % b == 0 && i != 0) {
            for (int k = 0; k < lineLength; k++)
                putchar('-');
            putchar('\n');
        }

        for (int j = 0; j < n; j++) {
            if (j % b == 0 && j != 0)
                printf("| ");

            int v = board->values[i][j];
            if (v == 0) {
                printf(".  ");
            } else {
                printf("%2d ", v);
            }
        }
        putchar('\n');
    }
}

bool isValid(Board *board, int row, int col, int num) {
    for (int x = 0; x < board -> size; x++) {
        if (board->values[row][x] == num || board->values[x][col] == num)
            return false;
    }

    int startRow = row - row % board -> boxSize;
    int startCol = col - col %  board -> boxSize;

    for (int i = 0; i <  board -> boxSize; i++)
        for (int j = 0; j <  board -> boxSize; j++)
            if (board->values[i + startRow][j + startCol] == num)
                return false;

    return true;
}

bool isBoardFull(Board *board) {
    for (int i = 0; i < board -> size; i++)
        for (int j = 0; j < board -> size; j++)
            if (board->values[i][j] == 0)
                return false;
    return true;
}

void copyBoard(Board *src, Board *dest) {
    dest->size    = src->size;
    dest->boxSize = src->boxSize;

    for (int i = 0; i < src->size; i++)
        for (int j = 0; j < src->size; j++)
            dest->values[i][j] = src->values[i][j];
}

void removeNumbers(Board *board, int emptyCells) {
    int count = 0;
    int max_cells = board -> size * board -> size;

    if (emptyCells > max_cells * 0.75) { // max 75% planszy może być puste
        emptyCells = (int)(max_cells * 0.75);
    }
    while (count < emptyCells) {
        int row = rand() % board -> size;
        int col = rand() % board -> size;

        if (board->values[row][col] != 0) {
            board->values[row][col] = 0;
            count++;
        }
    }
}

