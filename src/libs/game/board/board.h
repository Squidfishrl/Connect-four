/* IMPORTED LIBRARIES */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "../settings/settings.h"

/* -------------------------------------------------------------------------- */


/* STRUCTURE DEFINITIONS */

struct node_t{

    short row;
    short column;

    struct node_t* up;
    struct node_t* down;
    struct node_t* left;
    struct node_t* right;

    short type;
    bool winHiglight;
    /*
    0 - empty;
    1 - player 1
    2 - player 2
    */
};



struct matrix_t{

    short rows;
    short columns;

    struct node_t*** nodeMap; // 2d pointer array [row][column] (0,0) is the bottom left
};

/* -------------------------------------------------------------------------- */


/* FUNCTION DECLARATIONS */

struct node_t* get_node_by_cords(struct matrix_t* matrix, int row, int column);
void print_matrix(struct matrix_t* matrix, struct settings_t* settings, struct dict_t* colourDict);
void free_matrix(struct matrix_t* matrix);
struct matrix_t* create_matrix(int rows, int columns);
struct matrix_t* init_matrix(int rows, int columns); // help function for create_matrix
struct node_t* init_node(int row, int column, struct node_t* up, struct node_t* down, struct node_t* left, struct node_t* right);


// not needed
//struct matrix_t* clone_matrix(struct matrix_t* matrix);
// void basic_print_matrix(struct matrix_t* matrix); // for debugging
// void matrix_to_array(struct matrix_t* matrix, short arr[][matrix->columns]); // savestates for matrix when evaluating -> adds only values
// void array_to_matrix(struct matrix_t* matrix, short arr[][matrix->columns]); // changes values only

/* -------------------------------------------------------------------------- */


/* FUNCTION DEFINITIONS */


// O(1)
struct node_t* get_node_by_cords(struct matrix_t* matrix, int row, int column){

    return matrix->nodeMap[row][column];
}

// O(kn) k = rows, n = columns in a matrix
void print_matrix(struct matrix_t* matrix, struct settings_t* settings,struct dict_t* colourDict){

    log_stderr(0, 1, "Printing board");
    // fetch colours
    log_stderr(0, 0, "Fetching colours from colour dict");

    const char* def = binary_search_dict('D', colourDict);
    const char* winHighlightColour = binary_search_dict(settings->gameSettings.winHighlightColour, colourDict);
    const char* flash = binary_search_dict('F', colourDict);
    const char* heavy = binary_search_dict('H', colourDict);
    const char* yellow = binary_search_dict('Y', colourDict);

    system("clear");

    printf("%s%s CONNECT FOUR %s", heavy, yellow, def);
    printf("%s (in-game) %s\n\n", yellow, def);

    // add numbers above the board
    printf("|");
    for(int i = 1; i <= matrix->columns; i++){
        if(i > 9){
            printf("%c|", i+55);
        }else{
            printf("%d|", i);
        }

    }
    printf("\n");

    // iterate through rows
    for(struct node_t* iterNode1 = get_node_by_cords(matrix, matrix->rows-1, 0); iterNode1 != NULL; iterNode1 = iterNode1->down){

        // iterate through the columns
        for(struct node_t* iterNode2 = iterNode1; iterNode2 != NULL; iterNode2 = iterNode2->right){
            if(iterNode2->column == 0){
                printf("|");
            }

            // if empty nodes(no pieces)
            if(iterNode2->type == 0){
                printf(" ");
            }else if(iterNode2->type <= settings->playerSettings->playerArrSize){
                // if winning node
                if(iterNode2->winHiglight){
                    //print colour->flash->symbol
                    printf("%s%s%c", winHighlightColour, flash, settings->playerSettings->playerSettings[iterNode2->type - 1]->symbol);
                }else{
                    // print colour -> symbol
                    printf("%s%c", settings->playerSettings->playerSettings[iterNode2->type - 1]->colourCode, settings->playerSettings->playerSettings[iterNode2->type - 1]->symbol);
                }
            }else{
                printf("ERROR: player amount > playerArrSize");
                log_stderr(0, 3, "Player amount larger than player array size");
            }

            printf("%s", def); // stop colouring
            printf("|");
        }

        printf("\n");
    }

    // add numbers under the board
    printf("|");
    for(int i = 1; i <= matrix->columns; i++){
        if(i > 9){
            printf("%c|", i+55);
        }else{
            printf("%d|", i);
        }
    }
    printf("\n");
}

// O(kn) k = rows, n = columns in a matrix
void free_matrix(struct matrix_t* matrix){

    char logMsg[50];
    log_stderr(0, 1, "Freeing board");

    if(matrix != NULL){

        if(matrix->nodeMap != NULL){

            struct node_t* columnIterNode = get_node_by_cords(matrix, 0, 0);
            struct node_t* holdNode;

            // iterate left->right and then move up

            //iterate through rows
            for(struct node_t* rowIterNode = get_node_by_cords(matrix, 0, 0); rowIterNode != NULL;){

                // move up - doing it early so that rowIterNode isn't affected by the frees
                rowIterNode = rowIterNode->up;

                // iterate through columns
                while(columnIterNode != NULL){

                    // free current node
                    holdNode = columnIterNode->right;
                    sprintf(logMsg, "Freeing node [row-column] [%d %d]", columnIterNode->row, columnIterNode->column);
                    log_stderr(0, 0, logMsg);
                    free(columnIterNode);
                    columnIterNode = holdNode;
                }

                columnIterNode = rowIterNode;
            }

            // free 2d aspect of nodeMap
            // if board is
            /*
            0 0 0 0
            0 0 0 0
            0 0 0 0
            0 0 0 0
            */
            // free board to where it looks like
            /*
            0
            0
            0
            0
            */
            log_stderr(0, 0, "Freeing 2d aspect of nodemap");
            for(int i =0; i<matrix->rows; i++){
                sprintf(logMsg, "Freeing column space at row %d", i);
                log_stderr(0, 0, logMsg);
                free(matrix->nodeMap[i]);
            }

        }else{
            log_stderr(0, 2, "Matrix nodemap is already NULL");
        }

    //free nodeMap - 1d aspect
    log_stderr(0, 0, "Freeing matrix nodemap");
    free(matrix->nodeMap);

    }else{
        log_stderr(0, 2, "Matrix is already NULL");
    }

    // free matrix
    log_stderr(0, 0, "Freeing matrix");
    free(matrix);
}

// O(kn) assuming malloc is O(n) and k is the amount of nodes the matrix should contain(rows*columns)
struct matrix_t* create_matrix(int rows, int columns){

    // init matrix
    struct matrix_t* newMatrix = init_matrix(rows, columns);
    if(newMatrix == NULL){
        log_stderr(0, 3, "Failed creating matrix");
        return NULL;
    }

    log_stderr(0, 0, "Adding bottom left node");

    struct node_t* bottomLeftNode = init_node(0, 0, NULL, NULL, NULL, NULL);
    if(bottomLeftNode == NULL){
        log_stderr(0, 3, "Failed initializing node");
        free_matrix(newMatrix);
        return NULL;
    }

    struct node_t* tempNode1 = bottomLeftNode;
    struct node_t* tempNode2 = bottomLeftNode;

    // iterate through the columns
    for(int column = 0; column < columns; column++){

        // add to matrix
        newMatrix->nodeMap[0][column] = tempNode2;

        // iterate through the rows
        for(int row = 1; row < rows; row++){

            // case for leftmost nodes
            if(column == 0){

                // 2 way link current node and node above it
                log_stderr(0, 0, "Adding node above");
                tempNode1->up = init_node(row, column, NULL, tempNode1, NULL, NULL);
                if(tempNode1->up == NULL){
                    log_stderr(0, 3, "Failed initializing node");
                    free_matrix(newMatrix);
                    return NULL;
                }

            }else{

                log_stderr(0, 0, "Adding node above");

                // 2 way link current node and node above it. Also 2 way link above node->left with above node
                tempNode1->up = init_node(row, column, NULL, tempNode1, tempNode1->left->up, NULL);

                if(tempNode1->up == NULL){
                    log_stderr(0, 3, "Failed initializing node");
                    free_matrix(newMatrix);
                    return NULL;
                }

                tempNode1->left->up->right = tempNode1->up;
            }

            // move up
            tempNode1 = tempNode1->up;

            // add to matrix
            newMatrix->nodeMap[row][column] = tempNode1; // <- here
        }

        // 2 way link bottommost node[current column] to node->right
        log_stderr(0, 0, "Adding bottommost node to the right");
        tempNode2->right = init_node(0, column+1, NULL, NULL, tempNode2, NULL);

        if(tempNode2->right == NULL){
            log_stderr(0, 3, "Failed initializing node");
            free(newMatrix);
            return NULL;
        }

        // move right and prepare for another iteration
        tempNode2 = tempNode2->right;
        tempNode1 = tempNode2;
    }

    // for some reason there is always 1 extra node on row 0 to the right so I remove it
    log_stderr(0, 0, "Freeing excess node");
    free(newMatrix->nodeMap[0][columns-1]->right);
    get_node_by_cords(newMatrix, 0, columns-1)->right = NULL;

    return newMatrix;
}

struct matrix_t* init_matrix(int rows, int columns){

    log_stderr(0, 1, "Initializing board");
    char logMsg[70];

    // create matrix and link to the node network
    struct matrix_t* newMatrix = (struct matrix_t*)malloc(sizeof(struct matrix_t));

    if(newMatrix == NULL){
        log_stderr(0, 3, "Allocating memory for matrix failed");
        return NULL;
    }else{
        log_stderr(0, 0, "Successfully allocated memory for matrix");
    }

    // allocate first dimension for the matrix [row amount]
    newMatrix->nodeMap = (struct node_t***)malloc(rows * sizeof(struct node_t**));
    if(newMatrix->nodeMap == NULL){
        log_stderr(0, 3, "Allocating memory for matrix->nodeMap [rows] failed");

        free_matrix(newMatrix);
        return NULL;
    }else{
        log_stderr(0, 0, "Successfully allocated memory for matrix->nodeMap [row]");
    }

    // allocate second dimension for the matrix [column amount]
    for(short i = 0; i<rows; i++){
        newMatrix->nodeMap[i] = (struct node_t**)malloc(columns*sizeof(struct node_t*));
        if(newMatrix == NULL){
            sprintf(logMsg, "Allocating memory for matrix array [column] %hd failed", i);
            log_stderr(0, 3, logMsg);
            free_matrix(newMatrix);
            return NULL;
        }else{
            sprintf(logMsg, "Successfully allocated memory for matrix array [column] node %hd", i);
            log_stderr(0, 0, logMsg);
        }
    }

    newMatrix->rows = rows;
    newMatrix->columns = columns;

    return newMatrix;
}
// O(n), although time complexity of malloc is rather non-deterministic
struct node_t* init_node(int row, int column, struct node_t* up, struct node_t* down, struct node_t* left, struct node_t* right){

    struct node_t* initNode = (struct node_t*)malloc(sizeof(struct node_t));
    if(initNode == NULL){
        log_stderr(0, 3, "Allocating memory for matrix node failed");
        return NULL;
    }else{
        log_stderr(0, 0, "Successfully allocated memory for matrix node");
    }

    initNode->row = row;
    initNode->column = column;
    initNode->up = up;
    initNode->down = down;
    initNode->left = left;
    initNode->right = right;
    initNode->type = 0;
    initNode->winHiglight = false;

    return initNode;
}

// struct matrix_t* clone_matrix(struct matrix_t* matrix)
// {
// 	struct matrix_t* new_matrix = create_matrix(matrix->rows, matrix->columns);
// 	for (int y = 0; y < matrix->rows; y++)
// 	{
// 		for (int x = 0; x < matrix->columns; x++)
// 		{
// 			get_node_by_cords(new_matrix, y, x)->type = get_node_by_cords(matrix, y, x)->type;
// 		}
// 	}
//
// 	return new_matrix;
// }

/* -------------------------------------------------------------------------- */

// // O(kn) k = rows, n = columns in a matrix
// void array_to_matrix(struct matrix_t* matrix, short arr[][matrix->columns]){
//
//     // iterate through the rows
//     for(short i = 0; i<matrix->rows; i++){
//
//         // iterate through the columns
//         for(short b = 0; b<matrix->columns; b++){
//             // assign arr values to matrix nodes
//             matrix->nodeMap[i][b]->type = arr[i][b];
//         }
//     }
// }
//
// // O(kn) k = rows, n = columns in a matrix
// void matrix_to_array(struct matrix_t* matrix, short arr[][matrix->columns]){
//
//     // iterate through the rows
//     for(short i = 0; i < matrix->rows; i++){
//
//         // iterate through the columns
//         for(short b = 0; b<matrix->columns; b++){
//
//             // assign node values to arr
//             arr[i][b] = matrix->nodeMap[i][b]->type;
//         }
//     }
// }
//
// // O(kn) k = rows, n = columns in a matrix

// void basic_print_matrix(struct matrix_t* matrix){
//
//     for(struct node_t* iterNode1 = get_node_by_cords(matrix, matrix->rows-1, 0); iterNode1 != NULL; iterNode1 = iterNode1->down){
//
//         // iterate through the columns
//         for(struct node_t* iterNode2 = iterNode1; iterNode2 != NULL; iterNode2 = iterNode2->right){
//             if(iterNode2->column == 0){
//                 printf("|");
//             }
//
//             // if empty nodes(no pieces)
//             if(iterNode2->type == 0){
//                 printf(" ");
//             }else if(iterNode2->type <= 2){
//                 if(iterNode2->type == 1){
//                     printf("X");
//                 }else if(iterNode2->type == 2){
//                     printf("O");
//                 }
//
//             }
//
//             printf("|");
//         }
//
//         printf("\n");
//     }
//
// }
