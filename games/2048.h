#pragma once



typedef struct Board {
    int grid[4][4];
} Board;

void initBoard(Board *board);
void printBoard(Board *board);
Board start_game();
void addRandom(Board *board);
int isFull(Board *board);
int isMovePossible(Board *board);
int moveLeft(Board *board);
int moveRight(Board *board);
int moveUp(Board *board);
int moveDown(Board *board);

