/* IMPORTED LIBRARIES */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#include "../stats/stats.h" // has all the other headers included


/* -------------------------------------------------------------------------- */


/* FUNCTION DECLARATIONS */

void game_loop(struct settings_t* settings, struct stats_t* stats, struct dict_t* colourDict);
short add_piece(struct matrix_t* matrix, short player, short position);
short check_win(struct matrix_t* matrix, short player, short position, short connectAmount);
void log_moves(struct matrix_t* matrix, short moves[], short max_moves, char *log_name, struct playerSettings_t* settings);
short bot_move(struct matrix_t* matrix, short player, short current_move, short max_moves, struct settings_t* settings, struct dict_t* colourDict);
bool bot_can_play(struct matrix_t* matrix, short position);
bool bot_winning_move(struct matrix_t* matrix, short player, short position, struct settings_t* settings);
int negamax(struct matrix_t* matrix, short player, short current_move, short max_moves, struct settings_t* settings, int alpha, int beta);


/* -------------------------------------------------------------------------- */


/* FUNCTION DEFINITIONS */

void game_loop(struct settings_t* settings, struct stats_t* stats, struct dict_t* colourDict)
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
    char stats_name[settings->fileSettings.statsFileNameLen + strlen("../res/bin/")];
    sprintf(stats_name, "../res/bin/%s", settings->fileSettings.statsFileName);

	short player = 1;
	short position = 0;

	const short max_moves = matrix->rows * matrix->columns;

	short moves[max_moves];
    memset(moves, 0, max_moves*sizeof(short));

    stats->total_games++;
    for (int x = 0; x < settings->gameSettings.playerAmount; x++)
	{
		stats->player[x].games++;
	}

    clock_t start, stop;
    long long total_duration = 0;

	// Get player input - Done
	// Verify input - Done
	// Add piece - Done
	// Log move - Done
	// Display matrix - Done
	// Check win - Done

	start = clock();

    print_matrix(matrix, settings, colourDict);

	for (short i = 0; 1; player = 1 + (player >= settings->gameSettings.playerAmount ? 0 : player), i++)
	{
        while (1)
		{
            printf("%d/%d \n", i, max_moves);
			printf("Player %hd: ", player);

         if(settings->gameSettings.againstBot == false){
            log_stderr(0, 0, "Ask player for column pick");
                while(get_short_from_char(&position, 1, matrix->columns, "Invalid position!\nchoose column: ") != true){
                // msg repeating because if get_short exits from esc it won't say any msg
                    printf("Invalid position!\nchoose column: ");
                };
         }else{
              if(player > 1){
                  position = bot_move(matrix, player, i, max_moves, settings, colourDict);
              }
         }


				while(get_short_from_char(&position, 1, matrix->columns, "Invalid position!\nChoose column: ") != true){
					// msg repeating because if get_short exits from esc it won't say any msg
					printf("Invalid position!\nChoose column: ");
				};

				position--;
			}

			switch (add_piece(matrix, player, position))
			{
				case 0:		// Successful move
					break;

				case 1:		// Position exceeds board width
                    // shouldnt be possible with input validation added
					printf("Position out of bounds!\n");
          log_stderr(0, 2, "Position out of bounds");
					continue;

				case 2:		// Position exceeds board height
          
					printf("Position has too many pieces!\n");
          log_stderr(0, 2, " Column full - can't add piece");
					continue;

				default:	// Position is invalid for some other reason (unused?)
					printf("Position invalid!\n");
          log_stderr(0, 2, "Invalid position for unknown reasons");
					continue;
			}

			break;
		}

		moves[i] = position+1;
		stats->total_moves++;
		stats->player[player-1].moves++;

        print_matrix(matrix, settings, colourDict);

        stop = clock();

		long long duration = 1000.0 * (stop - start) / CLOCKS_PER_SEC - total_duration;
        stats->total_playtime += duration;
        for (int x = 0; x < settings->gameSettings.playerAmount; stats->player[x++].playtime += duration);
        total_duration += duration;

		write_stats_file(stats, stats_name);

        // win condition
    if(check_win(matrix, player, position, settings->gameSettings.connectAmount))
		{
      log_stderr(0, 1, "Game over - a player won");
			stats->total_wl++;
			stats->player[player-1].wins++;
			stats->player[player-1].losses--;
			for (int x = 0; x < settings->gameSettings.playerAmount; stats->player[x++].losses++);

            print_matrix(matrix, settings, colourDict); // to highlight winning nodes
            printf("Player %hd wins!\n", player);
            break;
    } // draw condition
        else if (i >= max_moves-1){
            log_stderr(0, 1, "Game over - a draw");
          	stats->total_draws++;
			      for (int x = 0; x < settings->gameSettings.playerAmount; stats->player[x++].draws++);
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
    write_stats_file(stats, stats_name);
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

short bot_move(struct matrix_t* matrix, short player, short current_move, short max_moves, struct settings_t* settings, struct dict_t* colourDict)
{
	int scores[matrix->columns];

	for (int x = 0; x < matrix->columns; x++)
	{
		if (bot_can_play(matrix, x))
		{
			printf("%d ", x);
			struct matrix_t* bot_matrix = clone_matrix(matrix);
			add_piece(bot_matrix, player, x);
			scores[x] = -negamax(bot_matrix, player, current_move + 1, max_moves, settings, -10000, 10000);
			free_matrix(bot_matrix);
		}
		else
		{
			scores[x] = -matrix->rows * matrix->columns - 1;
		}
	}

	int highest_score_x = 0;
	for (int x = 1, highest_score = scores[0]; x < matrix->columns; x++)
	{
		if (highest_score < scores[x])
		{
			highest_score = scores[x];
			highest_score_x = x;
		}
	}

	printf("\nScores: ");
	for (int x = 0; x < matrix->columns; x++)
	{
		printf("%d ", scores[x]);
	}
  getchar();
	printf("\n");

	return highest_score_x;
}

// Can the position be played
bool bot_can_play(struct matrix_t* matrix, short position)
{
	if (position < 0 || position >= matrix->columns)
	{
		return false;
	}

	return get_node_by_cords(matrix, matrix->rows-1, position)->type == 0;
}

// Would putting a piece in that position be a win
bool bot_winning_move(struct matrix_t* matrix, short player, short position, struct settings_t* settings)
{
	struct node_t* node = get_node_by_cords(matrix, 0, position);

	for(; node != NULL && node->type != 0; node = node->up);

	node->type = player;

	bool is_winning = check_win(matrix, player, position, settings->gameSettings.connectAmount);

	node->type = 0;

	return is_winning;
}

int negamax(struct matrix_t* matrix, short player, short current_move, short max_moves, struct settings_t* settings, int alpha, int beta)
{
	if (current_move >= max_moves)
	{
		return 0;
	}

	for (int x = 0; x < matrix->columns; x++)
	{
		if (bot_can_play(matrix, x) && bot_winning_move(matrix, player, x, settings))
		{
			return (matrix->rows * matrix->columns + 1 - current_move) / 2;
		}
	}

	int max_score = (matrix->rows * matrix->columns - 1 - current_move) / 2;
	if (beta > max_score)
	{
		beta = max_score;
		if (alpha >= beta)
		{
			return beta;
		}
	}

	for (int x = 0; x < matrix->columns; x++)
	{
		if (bot_can_play(matrix, x))
		{
			struct matrix_t* next_matrix = clone_matrix(matrix);
			short next_player = 1 + (player >= settings->gameSettings.playerAmount ? 0 : player);

			add_piece(next_matrix, next_player, x);
			int score = -negamax(next_matrix, next_player, current_move + 1, max_moves, settings, -alpha, -beta);

			free_matrix(next_matrix);

			if (score >= beta)
			{
				return score;
			}
			if (score > alpha)
			{
				alpha = score;
			}
		}
	}

	return alpha;
}

/* -------------------------------------------------------------------------- */
