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
    struct node_t* firstNode;  // first node that was created
    int rows;
    int columns;
};

/* -------------------------------------------------------------------------- */


/* FUNCTION DECLARATIONS */

struct matrix_t* create_matrix(int rows, int columns);
struct node_t* init_node(int row, int column, struct node_t* up, struct node_t* down, struct node_t* left, struct node_t* right);

/* -------------------------------------------------------------------------- */


/* FUNCTION DEFINITIONS */

// O(kn) assuming malloc is O(n) and k is the amount of nodes the matrix should contain(rows*columns)
struct matrix_t* create_matrix(int rows, int columns){

    struct node_t* headNode = init_node(0, 0, NULL, NULL, NULL, NULL);
    // headNode would be the bottommost and leftmost node

    struct node_t* tempNode1 = headNode;
    struct node_t* tempNode2 = headNode;

    // iterate through the columns
    for(int column = 0; column < columns; column++){
        // iterate through the rows
        for(int row = 1; row < rows; row++){


            if(column == 0){
                // case for leftmost nodes
                // 2 way link current node and node above it
                tempNode1->up = init_node(row, column, NULL, tempNode1, NULL, NULL);
            }else{
                // 2 way link current node and node above it. Also 2 way link abovenode->left with above node
                tempNode1->up = init_node(row, column, NULL, tempNode1, tempNode1->left->up, NULL);
                tempNode1->left->up->right = tempNode1->up;
            }

            // move up
            tempNode1 = tempNode1->up;
        }

        // 2 way link bottommost node[current column] to node->right
        tempNode2->right = init_node(0, column+1, NULL, NULL, tempNode2, NULL);

        // move right and prepare for another iteration
        tempNode2 = tempNode2->right;
        tempNode1 = tempNode2;
    }

    // create matrix and link to the node network
    struct matrix_t* newMatrix = (struct matrix_t*)malloc(sizeof(struct matrix_t));
    newMatrix->firstNode = headNode; // bottom-most and left-most node
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
