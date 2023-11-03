#pragma once 

typedef struct TTT_Board{ 
    int gridT[9];
    int placesLeft;
} TTT_Board;

void initTTT_Board(TTT_Board* bT);
void printTTT_Board(TTT_Board* bT);
int moveTTT(TTT_Board* bT, int place);