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

struct node_t* get_node_by_cords(struct matrix_t* matrix, int row, int column);
int get_distance(int rowA, int columnA, int rowB, int columnB); // help function for get_node_by_cords
int find_min(int* arr, int size); // help function for get_node_by_cords


void print_matrix(struct matrix_t* matrix);
void free_matrix(struct matrix_t* matrix);
struct matrix_t* create_matrix(int rows, int columns);
struct node_t* init_node(int row, int column, struct node_t* up, struct node_t* down, struct node_t* left, struct node_t* right);

/* -------------------------------------------------------------------------- */


/* FUNCTION DEFINITIONS */

// O(n) where n is the closest distance from the edge node to the searched for node
struct node_t* get_node_by_cords(struct matrix_t* matrix, int row, int column){

    struct node_t* searchNode;

    // check if cords are valid
    if(row > matrix->rows || row < 0){
        return NULL;
    }

    if(column > matrix->columns || column < 0){
        return NULL;
    }

    // I could automate vv but its only 4 numbs so it should be alright

    // find best node to start search from
    int blDist = get_distance(0, 0, row, column); // I could also use matrix->bottomLeftNode->row and column but it takes longer to write
    int brDist = get_distance(0, matrix->columns-1, row, column);
    int tlDist = get_distance(matrix->rows-1, 0, row ,column);
    int trDist = get_distance(matrix->rows-1, matrix->columns-1, row, column);

    int numbArr[] = {blDist, brDist, tlDist, trDist};
    int min = find_min(numbArr, 4);

    if(blDist == min){
        searchNode = matrix->bottomLeftNode;
    }else if(brDist == min){
        searchNode = matrix->bottomRightNode;
    }else if(tlDist == min){
        searchNode = matrix->topLeftNode;
    }else{
        searchNode = matrix->topRightNode;
    }

    while(row != searchNode->row){
        if(searchNode->row > row){
            searchNode = searchNode->down;
        }else if(searchNode->row < row){  // elif instead of if, cuz searchNode could become NULL and segmentation fault
            searchNode = searchNode->up;
        }
    }

    while(column != searchNode->column){
        if(searchNode->column > column){
            searchNode = searchNode->left;
        }else if(searchNode->column < column){
            searchNode = searchNode->right;
        }
    }

    return searchNode;
}

// O(n), where n is array size
int find_min(int* arr, int size){ // help function for get_node_by_cord

    // find largest number by putting all the numbs in an array and iterating through it
    int min = arr[0];

    for(int i = 1; i < size; i++){
        if(arr[i] < min){
            min = arr[i];
        }
    }

    return min;
}

// O(1)
int get_distance(int rowA, int columnA, int rowB, int columnB){
    // calculate the required moves to reach a point (withouth diagonals)
    int rowDist = rowA - rowB;
    if(rowDist < 0) rowDist *= -1;

    int columnDist = columnA - columnB;
    if(columnDist < 0) columnDist *= -1;

    return rowDist + columnDist;
}

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
    for(int column = 0; column < columns; column++){
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
                if(row == rows-1 && column == columns-1){
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

    // for some reason thare is always 1 extra node on row 0 to the right so I remove it
    free(newMatrix->bottomRightNode->right);
    newMatrix->bottomRightNode->right = NULL;

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
