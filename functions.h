#include <stdio.h>

#include "combList.h"
#include "arrayList.h"

#define MAX_WORD_LEN 10

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

struct node{
    char data;
    int **neighbors;
    int colored;
};
/*
 * File Functions: fileFunctions.c
 */
int linesInFile(FILE *);
void loadDictionary(FILE *, listAndLast*, int);

void getNeighbors(int, int, int**);
int getNeighborsNos(int, int);
int createNeighbors(int**, int, int);

void createGraph(struct node**, char**);

void linearCombSearch(struct node**, listAndLast*, combList*);
int btSearch(struct node**, char*, int, int, int);

void print(listAndLast*);

#endif
