/* IMPORTED LIBRARIES */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../stats/stats.h" // has all the other headers included

/* -------------------------------------------------------------------------- */


/* FUNCTION DECLARATIONS */

void game_loop(struct settings_t* settings, struct dict_t* colourDict);
short add_piece(struct matrix_t* matrix, short player, short position);
short check_win(struct matrix_t* matrix, short player, short position, short connectAmount);
void log_moves(struct matrix_t* matrix, short moves[], short max_moves, char *log_name, struct playerSettings_t* settings);

/* -------------------------------------------------------------------------- */


/* FUNCTION DEFINITIONS */

void game_loop(struct settings_t* settings, struct dict_t* colourDict)
{

    log_stderr(0, 1, "Starting game loop");

    struct matrix_t* matrix = create_matrix(settings->gameSettings.boardRows, settings->gameSettings.boardColumns);
    if(matrix == NULL){
        log_stderr(0, 3, "Failed creating matrix");
        return;
    }else{
        log_stderr(0, 0, "Successfully created matrix");
    }

    char log_name[settings->fileSettings.logFileNameLen + strlen("../res/")];
    sprintf(log_name, "../res/%s", settings->fileSettings.logFileName);
    // strcat(log_name, settings->fileSettings.logFileName);

	short player = 1;
	short position = 0;

	const short max_moves = matrix->rows * matrix->columns;

	short moves[max_moves];
    memset(moves, 0, max_moves*sizeof(short));


	// Get player input - Done
	// Verify input - Done
	// Add piece - Done
	// Log move - Done
	// Display matrix - Done
	// Check win - Done

    print_matrix(matrix, settings, colourDict);

	for (short i = 0; 1; player = 1 + (player >= settings->gameSettings.playerAmount ? 0 : player), i++)
	{
        while (1)
		{
            printf("%d/%d \n", i, max_moves);
			printf("Player %hd: ", player);

            log_stderr(0, 0, "Ask player for column pick");
            while(get_short_from_char(&position, 1, matrix->columns, "Invalid position!\nchoose column: ") != true){
                // msg repeating because if get_short exits from esc it won't say any msg
                printf("Invalid position!\nchoose column: ");
            };

            position--;

			switch (add_piece(matrix, player, position))
			{
				case 0:		// Successful move
					break;

				case 1:		// Position exceeds board width
                    // shouldnt be possible with input validation added
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

		moves[i] = position+1;


        print_matrix(matrix, settings, colourDict);



        // win condition
        if(check_win(matrix, player, position, settings->gameSettings.connectAmount)){

            log_stderr(0, 1, "Game over - a player won");
            print_matrix(matrix, settings, colourDict); // to highlight winning nodes
            printf("Player %hd wins!\n", player);
            break;
        } // draw condition
        else if (i == max_moves-1){
            log_stderr(0, 1, "Game over - a draw");
            print_matrix(matrix, settings, colourDict); // to hide log msg
            printf("No more moves - Draw!\n");
            break;
        }
	}

	// log_moves(matrix, moves, max_moves, log_name);
    printf("Press any key to continue: ");
    getchar();
    // log and free after getchar so that logging isnt shown on screen
    log_moves(matrix, moves, max_moves, log_name, settings->playerSettings);
    free_matrix(matrix);
	return;
}

short add_piece(struct matrix_t* matrix, short player, short position)
{

    log_stderr(0, 0, "Adding piece");

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

short check_win(struct matrix_t* matrix, short player, short position, short connectAmount)
{

    /* TODO: maybe add another argument struct node_t* node.
    its then pointed to the new piece. Could be useful to then pass it to check_win
    making it O(1) - even though there are some fors they iterate 7 times max(no matter the arguments)
    */

    log_stderr(0, 0, "Checking for win");

    struct node_t* start = get_node_by_cords(matrix, matrix->rows-1, position), *node;
    short piece_count;
    bool checkWin = false;


    // start = last placed node
    for (; start->down != NULL && start->type == 0; start = start->down);

    // up and down case
    for(piece_count = 1, node = start; node->up != NULL && node->up->type == player; node = node->up, piece_count++);
    for(node = start; node->down != NULL && node->down->type == player; node = node->down, piece_count++);
    if(piece_count >= connectAmount){
        // change win highlight
        node->winHiglight = true; // current node

        // linked nodes
        for(; node->up != NULL && node->up->type == player; node = node->up, node->winHiglight = true);
        // dont return straight away to colour double connections
        checkWin = true;
    }

    // left and right case
    for(piece_count = 1, node = start; node->left != NULL && node->left->type == player; node = node->left, piece_count++);
    for(node = start; node->right != NULL && node->right->type == player; node = node->right, piece_count++);
    if(piece_count >= connectAmount){
        // change win higlight
        node->winHiglight = true; // current node
        for(; node->left != NULL && node->left->type == player; node = node->left, node->winHiglight = true);
        checkWin = true;
    }

    // diagonal left right case
    for(piece_count = 1, node = start; (node->left != NULL && node->left->up != NULL) && node->left->up->type == player; node = node->left->up, piece_count++);
    for(node = start; (node->right != NULL && node->right->down != NULL) && node->right->down->type == player; node = node->right->down, piece_count++);
    if(piece_count >= connectAmount){
        // change win higlight
        node->winHiglight = true; // current node
        for(; (node->left != NULL && node->left->up != NULL) && node->left->up->type == player; node = node->left->up, node->winHiglight = true);
        checkWin = true;
    }

    // diagonal right left case
    for(piece_count = 1, node = start; (node->right != NULL && node->right->up != NULL) && node->right->up->type == player; node = node->right->up, piece_count++);
    for(node = start; (node->left != NULL && node->left->down != NULL) && node->left->down->type == player; node = node->left->down, piece_count++);
    if(piece_count >= connectAmount){
        // change win higlight
        node->winHiglight = true; // current node
        for(; (node->right != NULL && node->right->up != NULL) && node->right->up->type == player; node = node->right->up, node->winHiglight = true);
        checkWin = true;
    }

    return checkWin;
}

void log_moves(struct matrix_t* matrix, short moves[], short max_moves, char* log_name, struct playerSettings_t* settings)
{

    log_stderr(0, 1, "Logging moves");

    // printf("%s\n\n", log_name);
	FILE* log_file;

	if ((log_file = fopen(log_name, "a"))){
        log_stderr(0, 0, "Successfully opened log file");
    }		// Try to open file
	// else if((log_file = fopen(log_name, "w+")));	// File doesn't exist; try to create it (unneeded with "a" mode)
    else										// Can't create file
	{
        log_stderr(0, 3, "Failed to create or open log file");
		// printf("Failed to create or open log_file file!\n");
		return;
	}

	for (short x = 0; x < max_moves && moves[x] != 0 ; x++)
	{
        // log moves in order
		fprintf(log_file, "%hd;", moves[x]);
	}

	fprintf(log_file, "\n\n");

	// Copy of print_matrix but with fprintf()

    log_stderr(0, 0, "Fprinting matrix");
    // iterate through rows
    for(struct node_t* iterNode1 = get_node_by_cords(matrix, matrix->rows-1, 0); iterNode1 != NULL; iterNode1 = iterNode1->down){

        // iterate through the columns
        for(struct node_t* iterNode2 = iterNode1; iterNode2 != NULL; iterNode2 = iterNode2->right){
            if(iterNode2->column == 0){
                fprintf(log_file, "|");
            }

            if(iterNode2->type == 0){
                fprintf(log_file, " ");
            }else{
                fprintf(log_file, "%c", settings->playerSettings[iterNode2->type-1]->symbol);
            }


            fprintf(log_file, "|");
        }
        fprintf(log_file, "\n");
    }

    fprintf(log_file, "\n");

    fclose(log_file);

	return;
}

/* -------------------------------------------------------------------------- */
