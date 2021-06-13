/* IMPORTED LIBRARIES */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* -------------------------------------------------------------------------- */


/* STRUCTURE DEFINITIONS */

// essentially an array used like a dictionary, uses binary search
struct dictNode_t{
    const char key;
    const char* value;
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
struct dictNode_t* binary_search_dict(char key, struct dict_t* dict, short min, short max);
void sortDict(struct dict_t* dict);
void free_dict(struct dict_t* dict);

/* -------------------------------------------------------------------------- */


/* FUNCTION DEFINITIONS */

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
    add_node('Y', "\033[33m", dict);  // yellow
    add_node('S', "\033[34m", dict);  // sky blue
    add_node('M', "\033[35m", dict);  // magenta
    add_node('A', "\033[96m", dict);  // aqua
    add_node('L', "\033[92m", dict);  // lime
    add_node('W', "\033[97m", dict);  // white

    // misc
    add_node('F', "\033[5m", dict);  // flashing
    add_node('L', "\033[1m", dict);  // Larger font (bold)
    add_node('D', "\033[0m", dict); // Default/reset colour

    sortDict(dict);

    return dict;
}

void sortDict(struct dict_t* dict){
        // bubble sort, because I assume they will be inserted in a somewhat order and thats where bubble sort excells
    short swaps = 1; // assign value > 0 so that for doesnt end immediately
    for(short i = 0; i < dict->currentSize-1 && swaps != 0; i++){
        swaps = 0;
        for(short b = dict->currentSize-(1+i); b > 0; b--){
            if(dict->nodeArr[b].key < dict->nodeArr[b-1].key){
                //swap
                printf("swapping %c and %c \n", dict->nodeArr[b].key, dict->nodeArr[b-1].key);
                struct dictNode_t holdNode = dict->nodeArr[b];
                memcpy(&dict->nodeArr[b], &dict->nodeArr[b-1], sizeof(struct dictNode_t));
                memcpy(&dict->nodeArr[b-1], &holdNode, sizeof(struct dictNode_t));
                swaps++;
            }
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

struct dictNode_t* binary_search_dict(char key, struct dict_t* dict, short min, short max){

    // key doesnt exist in dict
    if(max < min){
        return NULL;
    }else{

        short mid = min + (max-min) / 2;

        // found key condition
        if(dict->nodeArr[mid].key == key){
            return &dict->nodeArr[mid];
        }else if(dict->nodeArr[mid].key < key){
            // set min to mid and repeat
            return binary_search_dict(key, dict, mid+1, max);
        }else{
            // set max to mid and repeat
            return binary_search_dict(key, dict, min, mid-1);
        }
    }
}

/* -------------------------------------------------------------------------- */
