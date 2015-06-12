#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arrayList.h"
#include "functions.h"

void loadDictionary(FILE *dictionary, listAndLast *list, int lines){
    int i;

    for(i = 0;i < lines;i++){
               
        char *dictArray = (char*)malloc(sizeof(char) * MAX_WORD_LEN);
        memset(dictArray, '\0', MAX_WORD_LEN);

        int bytes;
        fscanf(dictionary, "%s%n", dictArray, &bytes);

        if(bytes > MAX_WORD_LEN){

            fseek(dictionary, -bytes, SEEK_CUR);
            dictArray = (char*)malloc(sizeof(char) * bytes);
            dictArray[bytes] = '\0';
            fscanf(dictionary, "%s%n", dictArray, &bytes);
        }
        loadArrayList(list, dictArray, bytes);
    }   
    fclose(dictionary);

    return;
}

int linesInFile(FILE *fp){

    int lines;
    char ch; 

    while(!feof(fp)){
        char ch = fgetc(fp);
        if(ch == '\n')
            lines++;
    }   
    rewind(fp);

    return lines;
}

