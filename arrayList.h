#ifndef ARRAYLIST_H
#define ARRAYLIST_H

    typedef struct arrayList{
        char *string;
        int count;
        struct arrayList *next;
        struct arrayList *previous;
        char **result;
    }arrayList;

    /*
     * This structure contains the list created and the reference to 
     * the last positions of all the buckets
     */
    typedef struct listAndLast{
        arrayList **bucket;
        arrayList **lastNode;
    }listAndLast;

    listAndLast* initArrayList();
    void loadArrayList(listAndLast*, char*, int);
    void threadSort(listAndLast*);
    void *mergeSort(void*);
    void mergeArray(char**, char**, int*, int*, int, int, int);
    void mergeListToArray(char**, arrayList**, arrayList**, int, int, int);
    void oddOneOut(arrayList*, arrayList*,  int);
    void swap(char**, char**);
    void deleteALNode(arrayList**);
    void freeArrayList(listAndLast*);

#endif
