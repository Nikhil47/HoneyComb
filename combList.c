#include <stdio.h>
#include <stdlib.h>

#include "combList.h"

combList* initComb(){

    combList *retVal;
    int i;

    retVal = (combList*)malloc(sizeof(combList));
    retVal -> comb = (combUnit**)malloc(sizeof(combUnit*) * 26);
    retVal -> last = (combUnit**)malloc(sizeof(combUnit*) * 26);

    for(i = 0;i < 26;i++){
        retVal -> comb[i] = (combUnit*)malloc(sizeof(combUnit));
        retVal -> comb[i] -> count = 0;
        retVal -> comb[i] -> next = NULL;

        retVal -> last[i] = retVal -> comb[i];
    }

    return retVal;
}

void loadCombList(char **comb, combList *list, int layers){

    int i, j;

    for(i = 0;i < layers;i++){
        int cells = (6*i == 0)?1:6*i;
        for(j = 0;j < cells;j++){

            int bNo = comb[i][j] - 65;
            combInsert(list -> comb[bNo], list -> last[bNo], i, j);
        }
    }

    return;
}

void combInsert(combUnit *comb, combUnit *last, int layer, int cCLN){

    combUnit *insert = (combUnit*)malloc(sizeof(combUnit));
    insert -> position[0][0] = layer;
    insert -> position[1][0] = cCLN;
    insert -> next = NULL;

    last -> next = insert;
    last = insert;

    comb -> count++;

    return;
}

void freeCombList(combList *cList){

    int i;

    for(i = 0;i < 26;i++){
        if(cList -> comb[i] -> count != 0){
            combUnit **iterator, *toBeFreed;
            for(iterator = &(cList -> comb[i] -> next);*iterator != NULL;iterator = &((*iterator) -> next)){
                toBeFreed = *iterator;
                //*iterator = (*iterator) -> next;
                free(toBeFreed);
            }
        }
    }
    free(cList -> last);
    free(cList);
    return;
}
