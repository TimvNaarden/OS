#include "2048.h"

char* int_to_array(int N) {
    if (N < 10){
        char* result = (char*) mem_alloc(2 * sizeof(char));
        result[0] = N + 48;
        result[1] = '\0';
        return result;
    }
    int digits = 1;
    int m = N;

    while(m /= 10) {
        digits++;
    }

    char* result = (char*)mem_alloc((digits + 1) * sizeof(char));


    int index = digits -1;
    while(N != 0) {
        result[index--] = (N % 10) + '0';
        N /= 10;
    }


    result[digits] = '\0';
    return result;
}
void initBoard(Board *b) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4 ; j++) {
            b->grid[i][j] = 0;
        }
    }
    addRandom(b);
    addRandom(b);
}

void printBoard(Board *b) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4 ; j++) {
            char* print = int_to_array(b->grid[i][j]);
            print_string(print);
            print_string(" ");
            mem_free(print);
        }
        print_nl();
    }
}

unsigned int seed = 42;
int createRandom() {
    seed = (214013*seed+2531011);
    return (seed>>16)&0x7FFF; 
}
void addRandom(Board *b) {
    int x, y;
    do {
        x = createRandom() % 4;
        y = createRandom() % 4;
    } while (b->grid[x][y] != 0);
    b->grid[x][y] = createRandom() % 2 == 0? 2 : 4;
}

int isFull(Board *b) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4 ; j++) {
            if (b->grid[i][j] == 0) {
                return 0;
            }
        }
    }
    return 1;
}

int isMovePossible(Board *b) {
    if (!isFull(b)) {
        return 1;
    }
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 3 ; j++) {
            if (j != 3 && b->grid[i][j] == b->grid[i][j + 1]) {
                return 1;
            }
            if (j != 0 && b->grid[i][j] == b->grid[i][j - 1]) {
                return 1;
            }
            if (i != 3 && b->grid[i][j] == b->grid[i + 1][j]) {
                return 1;
            }
            if (i != 0 && b->grid[i][j] == b->grid[i - 1][j]) {
                return 1;
            }    
        }
    }
    return 0;
}

int moveLeft(Board *b) {
    if (!isMovePossible(b)) {
        print_string("Game Over\n");
        return 1;
    }
    int moved = 0;
    for (int i = 0; i < 4; i++) {
        for (int j = 1; j < 4; j++) {
            if (b->grid[i][j] == 0) {
                continue;
            }
            for (int k = j - 1; k >= 0; k--) {
                if (b->grid[i][k] == 0) {
                    b->grid[i][k] = b->grid[i][k + 1];
                    b->grid[i][k + 1] = 0;
                    moved = 1;
                } else if (b->grid[i][k] == b->grid[i][k + 1]) {
                    b->grid[i][k] *= 2;
                    b->grid[i][k + 1] = 0;
                    moved = 1;
                    break; 
                } else {
                    break; 
                }
            }
        }
    }
    if (moved) {
        addRandom(b);
        clear_screen();
        printBoard(b);
    }
    moved = 0;
    return 0;
}

int moveRight(Board *b) {
    if (!isMovePossible(b)) {
        print_string("Game Over\n");
        return 1;
    }
    int moved = 0;
    for (int i = 0; i < 4; i++) {
        for (int j = 2; j >= 0; j--) {
            if (b->grid[i][j] == 0) {
                continue;
            }
            for (int k = j + 1; k < 4; k++) {
                if (b->grid[i][k] == 0) {
                    b->grid[i][k] = b->grid[i][k - 1];
                    b->grid[i][k - 1] = 0;
                    moved = 1;
                } else if (b->grid[i][k] == b->grid[i][k - 1]) {
                    b->grid[i][k] *= 2;
                    b->grid[i][k - 1] = 0;
                    moved = 1;
                    break;
                } else {
                    break;
                }
            }
        }
    }
    if (moved) {
        addRandom(b);
        clear_screen();
        printBoard(b);
    }
    moved = 0;
    return 0;
}

int moveUp(Board *b) {
    if (!isMovePossible(b)) {
        print_string("Game Over\n");
        return 1;
    }
    int moved = 0;
    for (int j = 0; j < 4; j++) {
        for (int i = 1; i < 4; i++) {
            if (b->grid[i][j] == 0) {
                continue;
            }
            for (int k = i - 1; k >= 0; k--) {
                if (b->grid[k][j] == 0) {
                    b->grid[k][j] = b->grid[k + 1][j];
                    b->grid[k + 1][j] = 0;
                    moved = 1;
                } else if (b->grid[k][j] == b->grid[k + 1][j]) {
                    b->grid[k][j] *= 2;
                    b->grid[k + 1][j] = 0;
                    moved = 1;
                    break;
                } else {
                    break;
                }
            }
        }
    }
    if (moved) {
        addRandom(b);
        clear_screen();
        printBoard(b);
    }
    moved = 0;
    return 0;
}

int moveDown(Board *b) {
    if (!isMovePossible(b)) {
        print_string("Game Over\n");
        return 1;
    }
    int moved = 0;
    for (int j = 0; j < 4; j++) {
        for (int i = 2; i >= 0; i--) {
            if (b->grid[i][j] == 0) {
                continue;
            }
            for (int k = i + 1; k < 4; k++) {
                if (b->grid[k][j] == 0) {
                    b->grid[k][j] = b->grid[k - 1][j];
                    b->grid[k - 1][j] = 0;
                    moved = 1;
                } else if (b->grid[k][j] == b->grid[k - 1][j]) {
                    b->grid[k][j] *= 2;
                    b->grid[k - 1][j] = 0;
                    moved = 1;
                    break; 
                } else {
                    break; 
                }
            }
        }
    }
    if (moved) {
        addRandom(b);
        clear_screen();
        printBoard(b);
    }
    moved = 0;
    return 0;
}


 