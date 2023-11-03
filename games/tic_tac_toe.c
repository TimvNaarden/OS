#include "tic_tac_toe.h"


void initTTT_Board(TTT_Board* bT) {
    int i = 49;
    for(int j = 0; j < 9; j++){
        bT->gridT[j] = i;
        i++;
    }
    bT->placesLeft = 9;
}

void printTTT_Board(TTT_Board* bT) {
    for(int j = 0; j < 9; j++){
        char* result = (char*) mem_alloc(2 * sizeof(char));
        result[0] = bT->gridT[j];
        result[1] = '\0';
        if((j % 3) == 0 && j != 0){
            print_string("---------\n");
        }
        if(j != 2 && j != 5 && j != 8) {
            print_string(result);
            print_string(" | ");
        } else {
            print_string(result);
            print_nl();
        }
        mem_free(result);

    }
}
unsigned int seedTTT = 42;
int createRandomTTT() {
    seedTTT = (214013*seedTTT+2531011);
    return (seedTTT>>16)&0x7FFF; 
}
int moveTTT(TTT_Board* bT, int place){
    seedTTT = place;
    int gridPlace = place - 49;
    if(bT->gridT[gridPlace] != place){
        return 1;
    }
    bT->gridT[gridPlace] = 'X';
    bT->placesLeft -= 1;
    if(bT->placesLeft) {
        int botPlace;
        do{
            botPlace = createRandomTTT() % 9;
        } while (bT->gridT[botPlace] >= 60);
        bT->gridT[botPlace] = 'O';
        bT->placesLeft -= 1;
    }
    int won = hasWon(bT);
    if (won != 0){
        return won;
    }

    return 0;
}
int hasWon(TTT_Board* bT){
    if(bT->gridT[0] == bT->gridT[1] && bT->gridT[0] == bT->gridT[2]){
        return bT->gridT[0];
    }
    if(bT->gridT[3] == bT->gridT[4] && bT->gridT[3] == bT->gridT[5]){
        return bT->gridT[3];
    }
    if(bT->gridT[6] == bT->gridT[7] && bT->gridT[6] == bT->gridT[8]){
        return bT->gridT[6];
    }
    if(bT->gridT[0] == bT->gridT[3] && bT->gridT[0] == bT->gridT[6]){
        return bT->gridT[0];
    }
    if(bT->gridT[1] == bT->gridT[4] && bT->gridT[1] == bT->gridT[7]){
        return bT->gridT[1];
    }
    if(bT->gridT[2] == bT->gridT[5] && bT->gridT[2] == bT->gridT[8]){
        return bT->gridT[2];
    }
    if(bT->gridT[0] == bT->gridT[4] && bT->gridT[0] == bT->gridT[8]){
        return bT->gridT[0];
    }
    if(bT->gridT[6] == bT->gridT[4] && bT->gridT[6] == bT->gridT[2]){
        return bT->gridT[6];
    }
    return 0;
}