#ifndef COMBLIST_H
#define COMBLIST_H

typedef struct combUnit{
    unsigned int position[2][1];
    struct combUnit *next;
    unsigned int count;
}combUnit;

typedef struct combList{
    combUnit **comb;
    combUnit **last;
}combList;

combList* initComb();
void loadCombList(char**, combList*, int);
void combInsert(combUnit*, combUnit*, int, int);
void freeCombList(combList*);

#endif
