#ifndef SUDOKU_H
#define SUDOKU_H
#include <stdbool.h>

typedef struct {
    int **values;
    int size;
    int boxSize;
}Board;

bool initBoard(Board *board, int size);
void freeBoard(Board *board);

void printBoard(Board *board);
bool isValid(Board *board, int row, int col, int num);
bool isBoardFull(Board *board);
void copyBoard(Board *board, Board *dest);
void removeNumbers(Board *board, int emptyCells);


#endif
