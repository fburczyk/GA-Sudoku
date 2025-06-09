cmake_minimum_required(VERSION 3.31)
project(Sudoku_GA C)

set(CMAKE_C_STANDARD 23)

add_executable(Sudoku_GA main.c ga_solver.c sudoku.c generate.c)
