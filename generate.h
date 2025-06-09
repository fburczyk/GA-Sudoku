#ifndef GENERATE_H
#define GENERATE_H

#include "sudoku.h"

bool generateFullBoard(Board *board);
static bool fillBoard(Board *board, int row, int col);

#endif