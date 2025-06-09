#include "ga_solver.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

#define POP_SIZE       500
#define MAX_GENERATION 10000
#define TOURNAMENT_K   3
#define CROSSOVER_P    0.8
#define MUTATION_P     0.3

static Board population[POP_SIZE];
static int fitnessArr[POP_SIZE];
static bool fixedCell[9][9];

static int computeFitness(Board *b) {
    int fit = 0;
    int size = b->size, box = b->boxSize;
    int cnt[17];
    // wiersze
    for (int i = 0; i < size; i++) {
        memset(cnt, 0, sizeof(cnt));
        for (int j = 0; j < size; j++) cnt[b->values[i][j]]++;
        for (int v = 1; v <= size; v++) if (cnt[v] > 1) fit += cnt[v] - 1;
    }
    // kolumny
    for (int j = 0; j < size; j++) {
        memset(cnt, 0, sizeof(cnt));
        for (int i = 0; i < size; i++) cnt[b->values[i][j]]++;
        for (int v = 1; v <= size; v++) if (cnt[v] > 1) fit += cnt[v] - 1;
    }
    // bloki
    for (int br = 0; br < size; br += box) {
        for (int bc = 0; bc < size; bc += box) {
            memset(cnt, 0, sizeof(cnt));
            for (int i = 0; i < box; i++) for (int j = 0; j < box; j++) cnt[b->values[br+i][bc+j]]++;
            for (int v = 1; v <= size; v++) if (cnt[v] > 1) fit += cnt[v] - 1;
        }
    }
    return fit;
}

static void markFixed(Board *puz) {
    for (int i = 0; i < puz->size; i++)
        for (int j = 0; j < puz->size; j++)
            fixedCell[i][j] = (puz->values[i][j] != 0);
}

static void initPopulation(Board *puz) {
    int size = puz->size;
    markFixed(puz);
    for (int p = 0; p < POP_SIZE; p++) {
        initBoard(&population[p], size);
        population[p].boxSize = puz->boxSize;
        for (int i = 0; i < size; i++) {
            bool used[10] = { false };
            for (int j = 0; j < size; j++)
                if (fixedCell[i][j]) used[puz->values[i][j]] = true;
            int frees[16], cnt = 0;
            for (int v = 1; v <= size; v++) if (!used[v]) frees[cnt++] = v;
            for (int k = cnt - 1; k > 0; k--) {
                int r = rand() % (k + 1);
                int tmp = frees[k]; frees[k] = frees[r]; frees[r] = tmp;
            }
            int idx = 0;
            for (int j = 0; j < size; j++)
                population[p].values[i][j] = fixedCell[i][j] ? puz->values[i][j] : frees[idx++];
        }
    }
}

static int selectTournament() {
    int best = rand() % POP_SIZE;
    for (int k = 1; k < TOURNAMENT_K; k++) {
        int c = rand() % POP_SIZE;
        if (fitnessArr[c] < fitnessArr[best]) best = c;
    }
    return best;
}

static int selectRoulette() {
    int max = fitnessArr[0];
    for (int i = 1; i < POP_SIZE; i++) if (fitnessArr[i] > max) max = fitnessArr[i];
    long total = 0;
    for (int i = 0; i < POP_SIZE; i++) total += max - fitnessArr[i] + 1;
    long r = rand() % total;
    long accum = 0;
    for (int i = 0; i < POP_SIZE; i++) {
        accum += max - fitnessArr[i] + 1;
        if (r < accum) return i;
    }
    return POP_SIZE - 1;
}

static int selectElite() {
    int bestIndex = 0;
    int bestFit = fitnessArr[0];
    for (int i = 1; i < POP_SIZE; i++) {
        if (fitnessArr[i] < bestFit) {
            bestFit = fitnessArr[i];
            bestIndex = i;
        }
    }
    return bestIndex;
}

static void crossoverOne(Board *a, Board *b, Board *c1, Board *c2) {
    int size = a->size;
    int p = rand() % size;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (i <= p) {
                c1->values[i][j] = a->values[i][j];
                c2->values[i][j] = b->values[i][j];
            } else {
                c1->values[i][j] = b->values[i][j];
                c2->values[i][j] = a->values[i][j];
            }
        }
    }
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            if (fixedCell[i][j]) {
                c1->values[i][j] = a->values[i][j];
                c2->values[i][j] = a->values[i][j];
            }
}

static void crossoverTwo(Board *a, Board *b, Board *c1, Board *c2) {
    int size = a->size;
    int p1 = rand() % size;
    int p2 = rand() % size;
    if (p1 > p2) { int tmp = p1; p1 = p2; p2 = tmp; }
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (i <= p1 || i > p2) {
                c1->values[i][j] = a->values[i][j];
                c2->values[i][j] = b->values[i][j];
            } else {
                c1->values[i][j] = b->values[i][j];
                c2->values[i][j] = a->values[i][j];
            }
        }
    }
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            if (fixedCell[i][j]) {
                c1->values[i][j] = a->values[i][j];
                c2->values[i][j] = a->values[i][j];
            }
}

static void crossoverUniform(Board *a, Board *b, Board *c1, Board *c2) {
    int size = a->size;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (rand() % 2) {
                c1->values[i][j] = a->values[i][j];
                c2->values[i][j] = b->values[i][j]; // remove bug
            } else {
                c1->values[i][j] = b->values[i][j];
                c2->values[i][j] = a->values[i][j];
            }
            if (fixedCell[i][j]) {
                c1->values[i][j] = a->values[i][j];
                c2->values[i][j] = a->values[i][j];
            }
        }
    }
}

static void mutateSwap(Board *ind) {
    int size = ind->size;
    int r = rand() % size, c1, c2;
    do { c1 = rand() % size; } while (fixedCell[r][c1]);
    do { c2 = rand() % size; } while (fixedCell[r][c2] || c2 == c1);
    int tmp = ind->values[r][c1];
    ind->values[r][c1] = ind->values[r][c2];
    ind->values[r][c2] = tmp;
}

static void mutateScramble(Board *ind) {
    int size = ind->size;
    int r = rand() % size;
    int start = rand() % size;
    int len = rand() % (size - start) + 1;
    int temp[16];
    int cnt = 0;
    for (int j = start; j < start + len; j++)
        if (!fixedCell[r][j]) temp[cnt++] = ind->values[r][j];
    for (int k = cnt - 1; k > 0; k--) {
        int rr = rand() % (k + 1);
        int t = temp[k]; temp[k] = temp[rr]; temp[rr] = t;
    }
    int idx = 0;
    for (int j = start; j < start + len; j++)
        if (!fixedCell[r][j]) ind->values[r][j] = temp[idx++];
}

static void mutateInversion(Board *ind) {
    int size = ind->size;
    int r = rand() % size;
    int i1 = rand() % size;
    int i2 = rand() % size;
    if (i1 > i2) { int tmp = i1; i1 = i2; i2 = tmp; }
    while (fixedCell[r][i1]) i1 = rand() % size;
    while (fixedCell[r][i2] || i2 == i1) i2 = rand() % size;
    if (i1 > i2) { int tmp = i1; i1 = i2; i2 = tmp; }
    int len = (i2 - i1 + 1) / 2;
    for (int k = 0; k < len; k++) {
        int idx1 = i1 + k;
        int idx2 = i2 - k;
        int t = ind->values[r][idx1];
        ind->values[r][idx1] = ind->values[r][idx2];
        ind->values[r][idx2] = t;
    }
}

bool solveSudokuGA(Board *puz, Board *sol, SelectionType sel, CrossoverType cro, MutationType mut) {
    int size = puz->size;
    int generacja = 0;
    srand((unsigned)time(NULL));
    initPopulation(puz);

    Board newPop[POP_SIZE];
    for (int i = 0; i < POP_SIZE; i++) {
        initBoard(&newPop[i], size);
        newPop[i].boxSize = puz->boxSize;
    }
    for (int gen = 0; gen < MAX_GENERATION; gen++) {
        printf("\rPokolenie: %d", gen);
        generacja++;
        fflush(stdout);
        for (int i = 0; i < POP_SIZE; i++)
            fitnessArr[i] = computeFitness(&population[i]);
        for (int i = 0; i < POP_SIZE; i++) {
            if (fitnessArr[i] == 0) {
                copyBoard(&population[i], sol);
                printf("\n");
                return true;
            }
        }
        for (int i = 0; i < POP_SIZE; i += 2) {
            int p1, p2;
            if (sel == SELECTION_TOURNAMENT) {
                p1 = selectTournament();
                p2 = selectTournament();
            }
            else if (sel == SELECTION_ROULETTE) {
                p1 = selectRoulette();
                p2 = selectRoulette();
            }
            else {
                p1 = selectElite();
                p2 = selectElite();
            }

            if ((double)rand() / RAND_MAX < CROSSOVER_P) {
                switch (cro) {
                    case CROSSOVER_ONE_POINT:
                        crossoverOne(&population[p1], &population[p2], &newPop[i], &newPop[i+1]);
                        break;
                    case CROSSOVER_TWO_POINT:
                        crossoverTwo(&population[p1], &population[p2], &newPop[i], &newPop[i+1]);
                        break;
                    default:
                        crossoverUniform(&population[p1], &population[p2], &newPop[i], &newPop[i+1]);
                }
            } else {
                copyBoard(&population[p1], &newPop[i]);
                copyBoard(&population[p2], &newPop[i+1]);
            }
            if ((double)rand() / RAND_MAX < MUTATION_P) {
                if (mut == MUTATION_SWAP) mutateSwap(&newPop[i]);
                else if (mut == MUTATION_SCRAMBLE) mutateScramble(&newPop[i]);
                else mutateInversion(&newPop[i]);
            }
            if ((double)rand() / RAND_MAX < MUTATION_P) {
                if (mut == MUTATION_SWAP) mutateSwap(&newPop[i+1]);
                else if (mut == MUTATION_SCRAMBLE) mutateScramble(&newPop[i+1]);
                else mutateInversion(&newPop[i+1]);
            }
        }
        for (int i = 0; i < POP_SIZE; i++)
            copyBoard(&newPop[i], &population[i]);
    }
    printf("\n");
    return false;
}
