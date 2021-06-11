/* IMPORTED LIBRARIES */

#include <stdio.h>
#include <stdlib.h>
#ifndef MAX_PATH
#define MAX_PATH	(260)
#endif // MAX_PATH

/* -------------------------------------------------------------------------- */


/* FUNCTION DECLARATIONS */

void game_loop(struct matrix_t* matrix, char log_name[MAX_PATH]);
short add_piece(struct matrix_t* matrix, short player, short position);
short check_win(struct matrix_t* matrix, short player, short position);
void log_moves(struct matrix_t* matrix, short moves[], short max_moves, char log_name[MAX_PATH]);

/* -------------------------------------------------------------------------- */


/* FUNCTION DEFINITIONS */

void game_loop(struct matrix_t* matrix, char log_name[MAX_PATH])
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
	for (short i = 0; 1; player =  1 + !(player - 1))
	{
        while (1)
		{
			printf("Player %hd: ", player);
			scanf("%hd", &position);

			switch (add_piece(matrix, player, position-1))
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

		print_matrix(matrix);

		if (check_win(matrix, player, position))
		{
			printf("Player %hd wins!\n");
		}
		else if (i >= max_moves)
		{
			printf("No more moves - Draw!\n");
		}
	}

	log_moves(matrix, moves, max_moves, log_name);

	return;
}

short add_piece(struct matrix_t* matrix, short player, short position)
{
	if (position < 0 || position > matrix->rows)
	{
		return 1;
	}

	struct node_t* node = get_node_by_cords(matrix, position, 0);

	for(; node->type == 0 || node == NULL; node = node->up);

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
    struct node_t* start = get_node_by_cords(matrix, position, matrix->columns - 1), *node;
    short piece_count = 0;

    for (; start->type == 0; start = start->down);

    for (node = start, piece_count = 0; node != NULL && node->type == player; piece_count++, node = node->right->up);
    if (piece_count >= 4)
	{
		return 1;
	}

    for (node = start, piece_count = 0; node != NULL && node->type == player; piece_count++, node = node->right);
    if (piece_count >= 4)
	{
		return 1;
	}

    for (node = start, piece_count = 0; node != NULL && node->type == player; piece_count++, node = node->right->down);
    if (piece_count >= 4)
	{
		return 1;
	}

    for (node = start, piece_count = 0; node != NULL && node->type == player; piece_count++, node = node->down);
    if (piece_count >= 4)
	{
		return 1;
	}

    for (node = start, piece_count = 0; node != NULL && node->type == player; piece_count++, node = node->left->down);
    if (piece_count >= 4)
	{
		return 1;
	}

    for (node = start, piece_count = 0; node != NULL && node->type == player; piece_count++, node = node->left);
    if (piece_count >= 4)
	{
		return 1;
	}

    for (node = start, piece_count = 0; node != NULL && node->type == player; piece_count++, node = node->left->up);
    if (piece_count >= 4)
	{
		return 1;
	}

    return 0;
}

void log_moves(struct matrix_t* matrix, short moves[], short max_moves, char log_name[MAX_PATH])
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
    struct node_t* iterNode2 = iterNode1; // 2 lines instead of 1 cause lines are shorter

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

