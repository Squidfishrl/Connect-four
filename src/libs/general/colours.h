/* IMPORTED LIBRARIES */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* -------------------------------------------------------------------------- */


/* STRUCTURE DEFINITIONS */

// essentially an array used like a dictionary, uses binary search
struct dictNode_t{
    const char key;
    char* value;
};

struct dict_t{

    short maxSize;
    short currentSize;
    struct dictNode_t* nodeArr;
};


/* -------------------------------------------------------------------------- */


/* FUNCTION DECLARATIONS */

struct dict_t* init_dict(short maxSize);
struct dict_t* init_colour_dict();
struct dictNode_t* init_dict_node(char key, char* value, struct dictNode_t* next);
void add_node(char key, char* value, struct dict_t* dict);
char* binary_search_dict(char key, struct dict_t* dict);
void sort_dict(struct dict_t* dict);
void swap(struct dictNode_t* node1, struct dictNode_t* node2); // help function for sort_dict
void free_dict(struct dict_t* dict);

void print_colour_dict(struct dict_t* colourDict);
/* -------------------------------------------------------------------------- */


/* FUNCTION DEFINITIONS */

void print_colour_dict(struct dict_t* colourDict){

    for(short i = 0; i<colourDict->currentSize; i++){

        if(i+1 < 10){ // ifs to make the colourNO be on the same height
            printf("\n   %hd) %s %c \033[0m ", i+1, colourDict->nodeArr[i].value, colourDict->nodeArr[i].key);
        }else{
            printf("\n  %hd) %s %c \033[0m ", i+1, colourDict->nodeArr[i].value, colourDict->nodeArr[i].key);
        }

    }
}

void free_dict(struct dict_t* dict){
    free(dict->nodeArr);
    free(dict);
}

struct dict_t* init_colour_dict(){
    struct dict_t* dict = init_dict(12);

    // colours
    add_node('B', "\033[30m", dict);  // black
    add_node('R', "\033[31m", dict);  // red
    add_node('G', "\033[32m", dict);  // green
    add_node('Y', "\033[93m", dict);  // yellow
    add_node('S', "\033[34m", dict);  // sky blue
    add_node('M', "\033[35m", dict);  // magenta
    add_node('A', "\033[96m", dict);  // aqua
    add_node('L', "\033[92m", dict);  // lime
    add_node('W', "\033[97m", dict);  // white

    // misc
    add_node('F', "\033[5m", dict);  // flashing
    add_node('H', "\033[1m", dict);  // Heavy font (bold)
    add_node('D', "\033[0m", dict); // Default/reset colour

    sort_dict(dict);

    return dict;
}

void swap(struct dictNode_t* node1, struct dictNode_t* node2){
    struct dictNode_t holdNode = *node1;
    memcpy(node1, node2, sizeof(struct dictNode_t));
    memcpy(node2, &holdNode, sizeof(struct dictNode_t));
}

void sort_dict(struct dict_t* dict){
        // bubble sort, because I assume they will be inserted in a somewhat order and thats where bubble sort excells
    bool swaps;
    for(short i = 0; i < dict->currentSize; i++){

        swaps = false;

        for(short b = 0; b < dict->currentSize - i - 1; b++){
            if(dict->nodeArr[b].key > dict->nodeArr[b+1].key){
                //swap
                swap(&dict->nodeArr[b], &dict->nodeArr[b+1]);
                // struct dictNode_t holdNode = dict->nodeArr[b];
                // memcpy(&dict->nodeArr[b], &dict->nodeArr[b-1], sizeof(struct dictNode_t));
                // memcpy(&dict->nodeArr[b-1], &holdNode, sizeof(struct dictNode_t));
                swaps = true;
            }
        }

        if(swaps == false){
            break;
        }
    }
}

struct dict_t* init_dict(short maxSize){

    struct dict_t* newDict = (struct dict_t*)malloc(sizeof(struct dict_t));
    newDict->maxSize = maxSize;
    newDict->currentSize = 0;
    newDict->nodeArr = (struct dictNode_t*)malloc(sizeof(struct dictNode_t) * maxSize);

    return newDict;
}

void add_node(char key, char* value, struct dict_t* dict){

    // if dict is full or key isnt a capital letter
    if(dict->maxSize == dict->currentSize || (key < 'A' || key > 'Z')){
        printf("dict max size reached / key isnt a capital letter");
        return;
    }else{
        struct dictNode_t node = {.key = key, .value = value};
        memcpy(&dict->nodeArr[dict->currentSize++], &node, sizeof(struct dictNode_t));
    }
}

char* binary_search_dict(char key, struct dict_t* dict){

    short min = 0;
    short max = dict->currentSize;
    short mid;
    // key doesnt exist in dict
    while(mid != max){
        // get middle
        mid = (max+min) / 2;

        // found key
        if(dict->nodeArr[mid].key == key){
            // printf("%c - %c", dict->nodeArr[mid].key, key);
            return dict->nodeArr[mid].value;
        }else if(dict->nodeArr[mid].key < key){
            // key isnt in the min half
            min = mid+1;
        }else{
            // key isnt in the max half
            max = mid-1;
        }
    }

    return NULL;
}

/* -------------------------------------------------------------------------- */
