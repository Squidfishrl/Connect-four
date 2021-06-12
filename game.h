/* IMPORTED LIBRARIES */

#include <stdio.h>
#include <stdlib.h>
#include "board.h"


/* -------------------------------------------------------------------------- */


/* FUNCTION DECLARATIONS */

void game_loop(struct matrix_t* matrix, char* log_name);
short add_piece(struct matrix_t* matrix, short player, short position);
short check_win(struct matrix_t* matrix, short player, short position);
void log_moves(struct matrix_t* matrix, short moves[], short max_moves, char *log_name);

/* -------------------------------------------------------------------------- */


/* FUNCTION DEFINITIONS */

void game_loop(struct matrix_t* matrix, char* log_name)
{
	short player = 1;
	short position = 0;

	const short max_moves = matrix->rows * matrix->columns;

	short moves[max_moves];
	for (short x = 0; x < max_moves; x++)
	{
		moves[x] = 0;
	}

	// Get player input - Done
	// Verify input - Done
	// Add piece - Done
	// Log move - Done
	// Display matrix - Done
	// Check win - Done

    // clear main menu
    print_matrix(matrix);

	for (short i = 0; 1; player =  1 + !(player - 1), i++)
	{
        while (1)
		{
			printf("Player %hd: ", player);
			scanf("%hd", &position);

            // clear board
          	position--;


			switch (add_piece(matrix, player, position))
			{
				case 0:		// Successful move
					break;

				case 1:		// Position exceeds board width
					printf("Position out of bounds!\n");
					continue;

				case 2:		// Position exceeds board height
					printf("Position has too many pieces!\n");
					continue;

				default:	// Position is invalid for some other reason (unused?)
					printf("Position invalid!\n");
					continue;
			}

			break;
		}

		moves[i] = position;

        printf("\n %d/%d \n\n", i+1, max_moves);
        print_matrix(matrix);

        // win condition
        if(check_win(matrix, player, position)){
            print_matrix(matrix); // to highlight winning nodes

            printf("Player %hd wins!\n", player);

            //eats scanf newline
            getchar();

            printf("Press any key to continue: ");
            getchar();
            break;
        } // draw condition
        else if (i == max_moves){
            printf("No more moves - Draw!\n");

            // eats scanf newline
            getchar();

            printf("Press any key to continue: ");
            getchar();
            break;
        }

		// print_matrix(matrix);

		// if (check_win(matrix, player, position))
		// {
		// 	printf("Player %hd wins!\n", player);
		// }
		// else

	}

	log_moves(matrix, moves, max_moves, log_name);

	return;
}

short add_piece(struct matrix_t* matrix, short player, short position)
{
	if (position < 0 || position >= matrix->columns)
	{
		return 1;
	}

	struct node_t* node = get_node_by_cords(matrix, 0, position);


	for(; node != NULL && node->type != 0; node = node->up);

	if (node == NULL)
	{
		return 2;
	}
	else
	{
		node->type = player;
	}

	return 0;
}

short check_win(struct matrix_t* matrix, short player, short position)
{
    struct node_t* start = get_node_by_cords(matrix, matrix->rows-1, position), *node;
    short piece_count;

    // start = last placed node
    for (; start->down != NULL && start->type == 0; start = start->down);
    printf("%hd, %hd \n", start->row, start->column);

    // up and down case
    for(piece_count = 1, node = start; node->up != NULL && node->up->type == player; node = node->up, piece_count++);
    for(node = start; node->down != NULL && node->down->type == player; node = node->down, piece_count++);
    if(piece_count >= 4){
        // change win highlight
        node->winHiglight = true; // current node

        // linked nodes
        for(; node->up != NULL && node->up->type == player; node = node->up, node->winHiglight = true);
        return 1;
    }

    // left and right case
    for(piece_count = 1, node = start; node->left != NULL && node->left->type == player; node = node->left, piece_count++);
    for(node = start; node->right != NULL && node->right->type == player; node = node->right, piece_count++);
    if(piece_count >= 4){
        // change win higlight
        node->winHiglight = true; // current node
        for(; node->left != NULL && node->left->type == player; node = node->left, node->winHiglight = true);
        return 1;
    }

    // diagonal left right case
    for(piece_count = 1, node = start; (node->left != NULL && node->left->up != NULL) && node->left->up->type == player; node = node->left->up, piece_count++);
    for(node = start; (node->right != NULL && node->right->down != NULL) && node->right->down->type == player; node = node->right->down, piece_count++);
    if(piece_count >= 4){
        // change win higlight
        node->winHiglight = true; // current node
        for(; (node->left != NULL && node->left->up != NULL) && node->left->up->type == player; node = node->left->up, node->winHiglight = true);
        return 1;
    }

    // diagonal right left case
    for(piece_count = 1, node = start; (node->right != NULL && node->right->up != NULL) && node->right->up->type == player; node = node->right->up, piece_count++);
    for(node = start; (node->left != NULL && node->left->down != NULL) && node->left->down->type == player; node = node->left->down, piece_count++);
    if(piece_count >= 4){
        // change win higlight
        node->winHiglight = true; // current node
        for(; (node->right != NULL && node->right->up != NULL) && node->right->up->type == player; node = node->right->up, node->winHiglight = true);
        return 1;
    }

    return 0;
}

void log_moves(struct matrix_t* matrix, short moves[], short max_moves, char* log_name)
{
	FILE* log;

	if ((log = fopen(log_name, "r+")));			// Try to open file
	else if ((log = fopen(log_name, "w+")));	// File doesn't exist; try to create it
	else										// Cannot create file
	{
		printf("Failed to create or open log file!\n");
		return;
	}

	fprintf(log, "\n");

	for (short x = 0; moves[x] != 0 && x < max_moves; x++)
	{
		if (x % 2 == 0)
		{
			fprintf(log, "%hd. ", x / 2 + 1);
		}

		fprintf(log, "%hd ", moves[x]);
	}

	fprintf(log, "\n\n");

	// Copy of print_matrix but with fprintf()
    struct node_t* iterNode1 = get_node_by_cords(matrix, matrix->rows-1, matrix->columns-1);
    struct node_t* iterNode2 = iterNode1;

    // iterate through rows
    for(int row = 0; row < matrix->rows; row++){

        for(int column = 0; column < matrix->columns; column++){
            fprintf(log, "|");

            switch(iterNode2->type){
                case 0:
                    fprintf(log, " ");
                    break;
                case 1:
                    fprintf(log, "X");
                    break;
                case 2:
                    fprintf(log, "O");
                    break;
            }

            fprintf(log, "|");
        }
        fprintf(log, "\n");
        iterNode1 = iterNode1->down;
        iterNode2 = iterNode1;
    }

    fprintf(log, "\n");

    fclose(log);

	return;
}

/* -------------------------------------------------------------------------- */
