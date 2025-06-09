#ifndef GA_SOLVER_H
#define GA_SOLVER_H

#include "sudoku.h"

typedef enum {
    SELECTION_TOURNAMENT,
    SELECTION_ROULETTE,
    SELECTION_ELITISM
} SelectionType;


typedef enum {
    CROSSOVER_ONE_POINT,
    CROSSOVER_TWO_POINT,
    CROSSOVER_UNIFORM
} CrossoverType;


typedef enum {
    MUTATION_SWAP,
    MUTATION_SCRAMBLE,
    MUTATION_INVERSION
} MutationType;

bool solveSudokuGA(Board *puzzle, Board *solution,
                   SelectionType sel,
                   CrossoverType cro,
                   MutationType mut);
#endif
