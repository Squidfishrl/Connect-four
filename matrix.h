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

    struct node_t* bottomLeftNode;  // first node that was created
    struct node_t* bottomRightNode;
    struct node_t* topLeftNode;
    struct node_t* topRightNode;

};

/* -------------------------------------------------------------------------- */


/* FUNCTION DECLARATIONS */

void print_matrix(struct matrix_t* matrix);
void free_matrix(struct matrix_t* matrix);
struct matrix_t* create_matrix(int rows, int columns);
struct node_t* init_node(int row, int column, struct node_t* up, struct node_t* down, struct node_t* left, struct node_t* right);

/* -------------------------------------------------------------------------- */


/* FUNCTION DEFINITIONS */

// O(n), where n is the amount of nodes connected to the matrix
void print_matrix(struct matrix_t* matrix){

    struct node_t* iterNode1 = matrix->topLeftNode;
    struct node_t* iterNode2 = matrix->topLeftNode;

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

    struct node_t* columnIterNode = matrix->bottomLeftNode;
    struct node_t* holdNode;

    // iterate left->right and then move up

    //iterate through rows
    for(struct node_t* rowIterNode = matrix->bottomLeftNode; rowIterNode != NULL;){

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

    free(matrix);
}

// O(kn) assuming malloc is O(n) and k is the amount of nodes the matrix should contain(rows*columns)
struct matrix_t* create_matrix(int rows, int columns){

    // create matrix and link to the node network
    struct matrix_t* newMatrix = (struct matrix_t*)malloc(sizeof(struct matrix_t));

    newMatrix->rows = rows;
    newMatrix->columns = columns;
    struct node_t* bottomLeftNode = init_node(0, 0, NULL, NULL, NULL, NULL);
    newMatrix->bottomLeftNode = bottomLeftNode;

    struct node_t* tempNode1 = bottomLeftNode;
    struct node_t* tempNode2 = bottomLeftNode;

    // iterate through the columns
    for(int column = 0; column < columns-1; column++){
        // iterate through the rows
        for(int row = 1; row < rows; row++){


            if(column == 0){
                // case for leftmost nodes
                // 2 way link current node and node above it
                tempNode1->up = init_node(row, column, NULL, tempNode1, NULL, NULL);

                // add topLeftNode to matrix
                if(row == rows-1){
                    newMatrix->topLeftNode = tempNode1->up;
                }

            }else{
                // 2 way link current node and node above it. Also 2 way link abovenode->left with above node
                tempNode1->up = init_node(row, column, NULL, tempNode1, tempNode1->left->up, NULL);
                tempNode1->left->up->right = tempNode1->up;

                // add topRightNode to matrix
                if(row == rows-1 && column == columns-2){
                    newMatrix->topRightNode = tempNode1->up;
                }
            }

            // move up
            tempNode1 = tempNode1->up;
        }

        // 2 way link bottommost node[current column] to node->right
        tempNode2->right = init_node(0, column+1, NULL, NULL, tempNode2, NULL);

        // add bottomRightNode to matrix
        if(column == columns-2){
            newMatrix->bottomRightNode = tempNode2->right;
        }

        // move right and prepare for another iteration
        tempNode2 = tempNode2->right;
        tempNode1 = tempNode2;
    }



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
