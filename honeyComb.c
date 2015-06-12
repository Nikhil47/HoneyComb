#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arrayList.h"
#include "combList.h"
#include "functions.h"

/*
 * The neighbors array contains the indices of its neighbors in the honeyComb.
 */

int totalLayers;

int main(int argc, char *argv[]){

    FILE *honeyComb, *dictionary;
    listAndLast *list;
    combList *cList;

    honeyComb = fopen(argv[1], "r");
    dictionary = fopen(argv[2], "r");

    if(honeyComb == NULL || dictionary == NULL){
        fprintf(stderr, "Can't open input files\n");
        exit(1);
    }

    fscanf(honeyComb, "%d", &totalLayers);
    char **comb = (char**)malloc(sizeof(char*) * totalLayers);

    int i;
    for(i = 0;i < totalLayers;i++){
        int cells = (6 * i == 0)?1:6 * i;
        comb[i] = (char*)malloc(sizeof(char) * cells);
        
        fscanf(honeyComb, "%s", comb[i]);
    }
    fclose(honeyComb);
   
    int lines = linesInFile(dictionary);
    
    cList = initComb();
    loadCombList(comb, cList, totalLayers);
    list = initArrayList();
    loadDictionary(dictionary, list, lines);

    struct node **graph = (struct node**)malloc(sizeof(struct node*) * totalLayers);

    for(i = 0;i < totalLayers;i++){
        int cells = (6 * i == 0)?1:6 * i;
        graph[i] = (struct node*)malloc(sizeof(struct node) * cells);
    }

    createGraph(graph, comb);
    free(comb);
    linearCombSearch(graph, list, cList);
    threadSort(list);
    print(list);

    free(graph);
    freeArrayList(list);
    freeCombList(cList);

    return(0);
}

void print(listAndLast *list){
    
    int i, j;

    for(i = 0;i < 26;i++){
        for(j = 0;j < list -> bucket[i] -> count;j++)
            printf("%s\n", list -> bucket[i] -> result[j]);
    }
    return;
}

/*
 * linearCombSearch: initiates a linear search first to look for first character match
 * and then launches a backtrack search for the words in dictionary
 */
void linearCombSearch(struct node **graph, listAndLast *dictionary, combList *cList){
    
    int i, j, k = 0;
    int found = 0, flag = 0;

    while(k != 26){
        arrayList *dictIter = dictionary -> bucket[k] -> next;
        combUnit *combIter = cList -> comb[k] -> next;

        if(combIter != NULL && dictIter != NULL){
            while(dictIter != NULL){
                while(combIter != NULL){

                    i = combIter -> position[0][0];
                    j = combIter -> position[1][0];

                    graph[i][j].colored = 1;

                    if(btSearch(graph, dictIter -> string, i, j, 1) == 1){
                        graph[i][j].colored = 0;
                        flag = 1;
                        break; 
                    }
                    graph[i][j].colored = 0;

                    combIter = combIter -> next;
                }

                if(!flag){
                    printf("To Be deleted: %s\n", dictIter -> string);
                    deleteALNode(&dictIter);
                    dictionary -> bucket[k] -> count--;
                }
                else{
                    dictIter = dictIter -> next;
                }
                combIter = cList -> comb[k] -> next;
                flag = 0;
            }
        }
        k++;
    }

    return;
}

/*
 * btSearch is back track search in the graph
 * l corresponds to layer
 * c corresponds to cCLN
 * word corresponds to the word being searched
 * marker corresponds to current matched position
 */
int btSearch(struct node **graph, char *string, int l, int c, int marker){
    
    int len = strlen(string);
    int nL, nC;                           /* nL: neighbor layer, nC: neighbor cCLN */
    int result = 0;

    /* When this process starts the previous character is already matched */
    int i;
    for(i = 0;i < getNeighborsNos(l, c);i++){
        
        nL = graph[l][c].neighbors[0][i];
        nC = graph[l][c].neighbors[1][i];

        if(graph[nL][nC].data == string[marker] && graph[nL][nC].colored == 0){
            graph[nL][nC].colored = 1;
            if(marker == (len - 1)){
                graph[nL][nC].colored = 0;
                return 1;
            }
            else{
                result = result || btSearch(graph, string, nL, nC, marker + 1);
                graph[nL][nC].colored = 0;
            }
        }
    }
    return result;
}

/*
 * This method is used to initialize the neighbors array in every graph node.
 */
void createGraph(struct node **graph, char **comb){
   
   int i, j; 
   for(i = 0;i < totalLayers;i++){
       int cells = (6 * i == 0)?1:6 * i;
       graph[i] = (struct node*)malloc(sizeof(struct node) * cells);
   }

   for(i = 0;i < totalLayers;i++){
       int cells = (6 * i == 0)?1:6 * i;
       for(j = 0;j < cells;j++){

           graph[i][j].data = comb[i][j];
           graph[i][j].neighbors = (int**)malloc(sizeof(int*) * 2);
           getNeighbors(i, j, graph[i][j].neighbors);
           graph[i][j].colored = 0;
       }
   }

   return;
}

/*
 * This function returns the number of neighbors which a node should have
 */
int getNeighborsNos(int layer, int cLCN){

    int len;
    
    if(layer == (totalLayers - 1)){
        if(cLCN % layer == 0)
            len = 3;
        else
            len = 4;
    }
    else
        len = 6;

    return len;
}

/* createNeighbors
 * This function is for creating the neighbors array according to the position
 * of the current node. 
 */
int createNeighbors(int **neighbors, int layer, int cLCN){
    
    int len;
    
    if(layer == (totalLayers - 1)){
        if(cLCN % layer == 0)
            len = 3;
        else
            len = 4;
    }
    else
        len = 6;

    neighbors[0] = (int*)malloc(sizeof(int) * len);
    neighbors[1] = (int*)malloc(sizeof(int) * len);

    return len;
}

/* getNeighbors
 * Get all the neighbors of the current cell
 * {This is an internal function which will be run for all the nodes}
 *
 * layer is the current layer           : Range [0, totalLayers)
 * cLCN is current layer cell number    : Range [0, 6*layer]
 * neighbors is the array which will contain the indices
 *
 */
void getNeighbors(int layer, int cLCN, int **neighbors){
    
    int k, i, mid, left;
    int len = createNeighbors(neighbors, layer, cLCN);

    /* If first element has been passed, then return all elements in next layer */
    if(layer == 0 && cLCN == 0){
        if(totalLayers > 1){
            
            for(k = 0;k < len;k++){
                neighbors[0][k] = 1;
                neighbors[1][k] = k;
            }
        }
    }
    else if(layer > 0 && layer < (totalLayers - 1)){
        /*
         * Step 1: If element number is divisible by the layer number then the cLCN
         * will have three neighbors, whose index positions are calculated by
         * the 'mid' formula
         *
         * Step 2: cLCN will have two neighbors in its own layer. Attention has to be paid
         * that indices of the arrays are not overshot.
         *
         * Step 3: cLCN will have only one neighbor in the previous layer.
         */
        if(cLCN % layer == 0){
            
            /* Step 1: find neighbors in next layer */
           
            mid = (int)(cLCN/layer) * (layer + 1);
            mid = mid - 2;
           
            for(i = 0;i < 3;i++){
                neighbors[0][i] = layer + 1;
                neighbors[1][i] = (++mid < 0)?mid + 6 * (layer + 1):mid; 
            }

            /* Step 2: Find neighbors in current layer */

            int mid = cLCN - 1;

            for(i = 3;i < 5;i++){
                neighbors[0][i] = layer;
                neighbors[1][i] = (mid < 0)?mid + 6 * (layer):mid % (6 * layer);
                mid += 2;
            }

            /* Step 3: Find neighbors in previous layer */
            
            mid = (int)(cLCN/layer) * (layer - 1);
            neighbors[0][5] = layer - 1;
            neighbors[1][5] = mid;
        }
        /*
         * Step 1: Otherwise the cLCN will have two neighbors from the next layer
         * whose indices will be given by the 'left' formula
         *
         * Step 2: cLCN will have two neighbors in the current layer
         *
         * Step 3: cLCN will have two neighbors from the previous layer
         */    
        else{

            /* Step 1: Find neighbors in next layer */
            left = (int)(cLCN/layer)*(layer+1) + cLCN % layer;
            left--;

            for(i = 0;i < 2;i++){
                neighbors[0][i] = layer + 1;
                neighbors[1][i] = (++left < 0)?left + 6 * (layer + 1):left % (6 * (layer + 1));
            }

            /* Step 2: Find neighbors in the current layer */
            
            left = cLCN - 1;

            for(i = 2;i < 4;i++){
                neighbors[0][i] = layer;
                neighbors[1][i] = (left < 0)?left + 6 * (layer):left % (6 * layer);
                left += 2;
            }

            /* Step 3: Find neighbors in the previous layer */

            if(layer - 1 >= 2)
                left = (int)(cLCN/layer) * (layer - 1) + cLCN % (layer - 1);
            else
                left = (int)(cLCN/layer) * (layer - 1) + cLCN % (layer - 1);

            left--;

            for(i = 4;i < 6;i++){
                neighbors[0][i] = layer - 1;
                neighbors[1][i] = (++left < 0)?left + 6 * (layer + 1):left % (6 * (layer - 1));
            }
        }
    }
    else{
    /*
     * This is the outermost layer case. On corners there will be only three neighbors
     * otherwise there will be four.
     */

        /* Step 1: Find neighbors in current layer */

        left = cLCN - 1;

        for(i = 0;i < 2;i++){
            neighbors[0][i] = layer;
            neighbors[1][i] = (left < 0)?left + 6 * (layer):left % (6 * layer);
            left += 2;
        }

        /* Step 2: Find neighbors in previous layer */

        if(cLCN % layer == 0){
            
            mid = (int)(cLCN/layer) * (layer - 1);
            neighbors[0][2] = layer - 1;
            neighbors[1][2] = mid;
        }
        else{
            
            mid = (int)(cLCN/layer) * (layer - 1) + cLCN % (layer - 1);
            mid--;

            for(i = 2;i < 4;i++){
                neighbors[0][i] = layer - 1;
                neighbors[1][i] = (++mid < 0)?mid + 6 * (layer + 1): mid % (6 * (layer - 1));
            }
        }
    }

    return;
}
