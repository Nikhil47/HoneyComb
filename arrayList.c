#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>

#include "arrayList.h"

typedef struct mergeSortArg{
    long threadID;
    arrayList *bucket;
    arrayList *lastNode;
}mergeSortArg;

listAndLast* initArrayList(){
    listAndLast *retVal;
    arrayList *bucket[26];
    int i;

    retVal = (listAndLast*)malloc(sizeof(listAndLast));
    retVal -> bucket = (arrayList**)malloc(sizeof(arrayList*) * 26); 
    retVal -> lastNode = (arrayList**)malloc(sizeof(arrayList*) * 26);

    for(i = 0;i < 26;i++){
        arrayList *node = (arrayList*)malloc(sizeof(arrayList));
        node -> string = NULL;
        node -> next = NULL;
        node -> previous = NULL;
        node -> count = 0;

        retVal -> bucket[i] = node;
        retVal -> lastNode[i] = retVal -> bucket[i];
    }

    return retVal;
}

void loadArrayList(listAndLast *list, char *string, int len){
    int bNo;     //bNo: Bucket Number
    arrayList *insert;

    bNo = string[0] - 65;

    insert = (arrayList*)malloc(sizeof(arrayList));
    insert -> string = (char*)malloc(sizeof(char) * len);
    insert -> next = NULL;
    free(insert -> result);

    strcpy(insert -> string, string);

    list -> lastNode[bNo] -> next = insert;
    insert -> previous = list -> lastNode[bNo];
    list -> lastNode[bNo] = insert;

    list -> bucket[bNo] -> count++;

    free(string);
    return;
}

void threadSort(listAndLast *list){
    
    long i;
    pthread_t thread[26];
    pthread_attr_t attr;
    void *status;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for(i = 0;i < 26;i++){
        mergeSortArg *arg = (mergeSortArg*)malloc(sizeof(mergeSortArg));
        arg -> bucket = list -> bucket[i];
        arg -> lastNode = list -> lastNode[i];
        arg -> threadID = i;

        if(pthread_create(&thread[i], &attr, mergeSort, (void *)arg)){
            fprintf(stderr, "Error in creating %ld thread\n", i);
        }
    }

    pthread_attr_destroy(&attr);

    for(i = 0;i < 26;i++){
        if(pthread_join(thread[i], &status)){
            fprintf(stderr, "Error in joining thread %ld\n", i);
        }
    }

    return;
}

void *mergeSort(void *arg){
    int i, j, k, n, len;
    mergeSortArg *argHolder = (mergeSortArg *) arg;

    arrayList *bucket = argHolder -> bucket;
    arrayList *lastNode = argHolder -> lastNode;
    long threadID = argHolder -> threadID;

    len = n = bucket -> count;

    if(n == 0)
        return;

    else if(n == 1){

        arrayList *toBeFreed;
        bucket -> result = (char**)malloc(sizeof(char*));
        bucket -> result[0] = bucket -> next -> string;
        toBeFreed = bucket -> next;
        bucket -> next = NULL;

        printf("%s\n", bucket -> result[0]);

        free(toBeFreed);
    }

    else{
        arrayList *fast, *slow;
        bucket -> result = (char**)malloc(sizeof(char*) * n);
        slow = fast = bucket -> next;

        for(i = 1;i < n;i *= 2){
            int slowP = 0, fastP = 0;
            char **ret;
         
            n = (n % 2)?n+1:n;
            int times = n/(2*i);
            times = (times)?times:1;

            printf("Initializing slow and fast: %d for %ld: %d\n", times, threadID, len);
    
            arrayList *temp;
            for(temp = fast;temp != NULL;temp = temp -> next)
                printf("%s\t", temp -> string);
            printf("\n");

            for(j = 0;j < times;j++){

                if(i == 1){
                    for(k = 0;k < i && fast -> next != NULL;k++)
                        fast = fast -> next;

                    mergeListToArray(bucket -> result, &slow, &fast, i, j, len);
                    slow = fast;
                }
                else{
                    for(k = 0;k < i && fastP < len;k++)
                        fastP++;

                    ret = (char**)malloc(sizeof(char*) * len);
                    mergeArray(bucket -> result, ret, &slowP, &fastP, i, j, len);
                    slowP = fastP;
                }
            }
            if(i != 1)
                bucket -> result = ret;
        }
    }
    return;
}

void mergeArray(char **array, char **ret, int *slow, int *fast, int step, int offset, int len){
    int k, i = *slow, fCntr = 0, sCntr = 0;

    char *garbage = (char*)malloc(sizeof(char) * 2);
    garbage[0] = '\0';
    garbage[1] = '\0';

printf("merge: %s\t%s\t%d\n", array[*slow], array[*fast], 2*step);

    while(i < (2 * step + 2 * offset) && i < len && *fast < len && *slow < len){
        
        if(fCntr == step || sCntr == step)
            break;

        else if(array[*fast][0] != '\0' && strcmp(array[*slow], array[*fast]) > 0){
            ret[i++] = array[(*fast)];
            array[*fast] = &garbage[0];

            if(*fast < len - 1){
                (*fast)++;
                fCntr++;
            }
        }
        
        else if(array[*slow][0] != '\0' && strcmp(array[*slow], array[*fast]) <= 0){
            ret[i++] = array[(*slow)];
            array[*slow] = &garbage[0];

            if(*slow < len - 1){
                (*slow)++;
                sCntr++;
            }
        }

        else{
            break;
        }
    }

    while(sCntr < step && i < len && *slow < len && array[*slow][0] != '\0'){
        ret[i++] = array[(*slow)++];
        sCntr++;
    }

    while(fCntr < step && i < len && *fast < len && array[*fast][0] != '\0'){
        ret[i++] = array[(*fast)++];
        fCntr++;
    }

    int leftover = *fast;

    while(i < len && leftover < len && array[leftover][0] != '\0'){
        ret[i++] = array[leftover++];
    }

    free(array);
    array = ret;
    for(k = 0;k < len;k++)
        printf("%s\t", array[k]);
    printf("\n");

    return;
}

void mergeListToArray(char **array, arrayList **slow, arrayList **fast, int step, int offset, int len){
    int i = 2*offset, sCntr = 0, fCntr = 0;
    arrayList *toBeFreed = NULL;

printf("merge: %s\t%s\t%d\n", (*slow) -> string, (*fast) -> string, 2*step);
    while(i < (2*step + 2*offset) && i < len){

        if(fCntr == step || sCntr == step)
            break;

        else if(strcmp((*slow) -> string, (*fast) -> string) > 0){
            if((*fast) != NULL){
                array[i] = (*fast) -> string;
                toBeFreed = *fast;
                (*fast) = (*fast) -> next;

                free(toBeFreed);
                fCntr++;
            }
        }
        else{
            if((*slow) != NULL){
                array[i] = (*slow) -> string;
                toBeFreed = *slow;
                (*slow) = (*slow) -> next;

                free(toBeFreed);
                sCntr++;
            }
        }
        i++;
    }

    while(fCntr < step && (*fast) != NULL && i < len){
        array[i++] = (*fast) -> string;
        toBeFreed = *fast;
        (*fast) = (*fast) -> next;

        free(toBeFreed);
        fCntr++;
    }

    while(sCntr < step && (*slow) != NULL && i < len){
        array[i++] = (*slow) -> string;
        toBeFreed = *slow;
        (*slow) = (*slow) -> next;

        free(toBeFreed);
        sCntr++;
    }

    return;
}

void deleteALNode(arrayList **node){

    arrayList *toBeFreed = *node;

    (*node) -> previous -> next = (*node) -> next;
    if((*node) -> next != NULL)
        (*node) -> next -> previous = (*node) -> previous;
    (*node) = toBeFreed -> next;

    free(toBeFreed -> string);
    free(toBeFreed);
        
    return;
}

void printArrayList(listAndLast *list){
    int i;

    for(i = 0;i < 26;i++){
        arrayList *iterator = list -> bucket[i];
        printf("Bucket: %d; Count: %d\n", i, iterator -> count);

        iterator = iterator -> next;
        while(iterator != NULL){
            printf("Bucket: %d; %s\n", i, iterator -> string);
            iterator = iterator -> next;
        }
        printf("\n");
    }

    return;
}

void freeArrayList(listAndLast *list){
    int i;

    for(i = 0;i < 26;i++){
        if(list -> bucket[i] -> count != 0)
            free(list -> bucket[i] -> result);
        free(list -> bucket[i]);
    }

    free(list);
    return;
}
