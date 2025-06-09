#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include "generate.h"
#include "sudoku.h"

static bool fillBoard(Board *board, int row, int col) {
    if (row == board -> size ) return true;

    int nextRow;
    if (col == board->size - 1) {
        nextRow = row + 1;
    } else {
        nextRow = row;
    }

    int nextCol;
    if (col == board->size - 1) {
        nextCol = 0;
    } else {
        nextCol = col + 1;
    }

    int* nums = (int*)malloc(board->size * sizeof(int));
    if (nums == NULL) {
        printf("Error: Failed to allocate memory for numbers array.\n");
        return false;
    }

    for (int i = 0; i < board->size; i++) nums[i] = i + 1;

    // Fisher-Yates algorithm to shuffle
    for (int i = board->size - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int tmp = nums[i];
        nums[i] = nums[j];
        nums[j] = tmp;
    }

    for (int i = 0; i < board->size; i++) {
        int num = nums[i];
        if (isValid(board, row, col, num)) {
            board->values[row][col] = num;
            if (fillBoard(board, nextRow, nextCol)) {
                free(nums);
                return true;
            }
            board->values[row][col] = 0;
        }
    }

    free(nums);
    return false;
}
bool generateFullBoard(Board *board) {
    board -> boxSize = (int)sqrt(board -> size);
    for (int i = 0; i < board->size; i++)
        for (int j = 0; j < board->size; j++)
            board->values[i][j] = 0;

    return fillBoard(board, 0, 0);
}