/* IMPORTED LIBRARIES */

#include <stdio.h>
#include <stdlib.h>

/* -------------------------------------------------------------------------- */


/* STRUCTURE DEFINITIONS */

struct node_t{

    int row;
    int column;

    struct node_t* up;
    struct node_t* down;
    struct node_t* left;
    struct node_t* right;

    int type;
    /*
    0 - empty;
    1 - player 1
    2 - player 2
    */
};



struct matrix_t{

    int rows;
    int columns;

    struct node_t*** nodeMap; // 2d pointer array [row][column] (0,0) is the bottom left
};

/* -------------------------------------------------------------------------- */


/* FUNCTION DECLARATIONS */

struct node_t* get_node_by_cords(struct matrix_t* matrix, int row, int column);
void print_matrix(struct matrix_t* matrix);
void free_matrix(struct matrix_t* matrix);
struct matrix_t* create_matrix(int rows, int columns);
struct matrix_t* init_matrix(int rows, int columns); // help function for create_matrix
struct node_t* init_node(int row, int column, struct node_t* up, struct node_t* down, struct node_t* left, struct node_t* right);

/* -------------------------------------------------------------------------- */


/* FUNCTION DEFINITIONS */

// O(1)
struct node_t* get_node_by_cords(struct matrix_t* matrix, int row, int column){

    return matrix->nodeMap[row][column];
}

// O(n), where n is the amount of nodes connected to the matrix
void print_matrix(struct matrix_t* matrix){

    struct node_t* iterNode1 = get_node_by_cords(matrix, matrix->rows-1, matrix->columns-1);
    struct node_t* iterNode2 = iterNode1; // 2 lines instead of 1 cuz lines are shorter

    // iterate through rows
    for(int row = 0; row < matrix->rows; row++){

        for(int column = 0; column < matrix->columns; column++){
            printf("|");

            switch(iterNode2->type){
                case 0:
                    printf(" ");
                    break;
                case 1:
                    printf("X");
                    break;
                case 2:
                    printf("O");
                    break;
            }

            printf("|");
        }
        printf("\n");
        iterNode1 = iterNode1->down;
        iterNode2 = iterNode1;
    }
}

// O(n), where n is the amount of nodes, connected to the matrix
void free_matrix(struct matrix_t* matrix){

    struct node_t* columnIterNode = get_node_by_cords(matrix, 0, 0);
    struct node_t* holdNode;

    // iterate left->right and then move up

    //iterate through rows
    for(struct node_t* rowIterNode = get_node_by_cords(matrix, 0, 0); rowIterNode != NULL;){

        // move up - doing it early so that rowIterNode isnt affected by the frees
        rowIterNode = rowIterNode->up;

        // iterate through columns
        while(columnIterNode != NULL){

            // free current node
            holdNode = columnIterNode->right;
            free(columnIterNode);
            columnIterNode = holdNode;
        }

        columnIterNode = rowIterNode;
    }

    // free 2d aspect of nodeMap
    for(int i =0; i<matrix->rows; i++){
        free(matrix->nodeMap[i]);
    }

    //free nodeMap and matrix struct
    free(matrix->nodeMap);
    free(matrix);
}

// O(kn) assuming malloc is O(n) and k is the amount of nodes the matrix should contain(rows*columns)
struct matrix_t* create_matrix(int rows, int columns){

    // init matrix
    struct matrix_t* newMatrix = init_matrix(rows, columns);

    struct node_t* bottomLeftNode = init_node(0, 0, NULL, NULL, NULL, NULL);

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
                tempNode1->up = init_node(row, column, NULL, tempNode1, NULL, NULL);

            }else{
                // 2 way link current node and node above it. Also 2 way link abovenode->left with above node
                tempNode1->up = init_node(row, column, NULL, tempNode1, tempNode1->left->up, NULL);
                tempNode1->left->up->right = tempNode1->up;
            }

            // move up
            tempNode1 = tempNode1->up;

            // add to matrix
            newMatrix->nodeMap[row][column] = tempNode1;
        }

        // 2 way link bottommost node[current column] to node->right
        tempNode2->right = init_node(0, column+1, NULL, NULL, tempNode2, NULL);

        // move right and prepare for another iteration
        tempNode2 = tempNode2->right;
        tempNode1 = tempNode2;
    }

    // for some reason thare is always 1 extra node on row 0 to the right so I remove it
    free(newMatrix->nodeMap[0][columns-1]->right);
    get_node_by_cords(newMatrix, 0, columns-1)->right = NULL;

    return newMatrix;
}

struct matrix_t* init_matrix(int rows, int columns){

    // create matrix and link to the node network
    struct matrix_t* newMatrix = (struct matrix_t*)malloc(sizeof(struct matrix_t));

    // allocate first dimension for the matrix
    newMatrix->nodeMap = (struct node_t***)malloc(rows * sizeof(struct node_t**));

    // allocate second dimension for the matrix
    for(int i = 0; i<columns; i++){
        newMatrix->nodeMap[i] = (struct node_t**)malloc(columns*sizeof(struct node_t*));
    }

    newMatrix->rows = rows;
    newMatrix->columns = columns;

    return newMatrix;
}
// O(n), althought time complexity of malloc is rather non-deterministic
struct node_t* init_node(int row, int column, struct node_t* up, struct node_t* down, struct node_t* left, struct node_t* right){

    struct node_t* initNode = (struct node_t*)malloc(sizeof(struct node_t));
    initNode->row = row;
    initNode->column = column;
    initNode->up = up;
    initNode->down = down;
    initNode->left = left;
    initNode->right = right;
    initNode->type = 0;

    return initNode;
}

/* -------------------------------------------------------------------------- */