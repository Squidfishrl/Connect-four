/* IMPORTED LIBRARIES */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h> // for stat tracking
#include <math.h> // for score evaluation

#include "../stats/stats.h" // has all the other headers included

/* -------------------------------------------------------------------------- */


/* STRUCTURE DEFINITIONS */



/* -------------------------------------------------------------------------- */


/* FUNCTION DECLARATIONS */

/* LOGIC */

void game_loop(struct settings_t* settings, struct stats_t* stats, struct dict_t* colourDict);
struct node_t* add_piece(struct matrix_t* matrix, short player, short position); // returns the changed node
bool check_win(struct node_t* start, short connectAmount, bool colour);
void log_moves(struct matrix_t* matrix, short moves[], short max_moves, char *log_name, struct playerSettings_t* settings);

/* BOT */

short bot_moves(struct matrix_t* board, short player, struct settings_t* settings);
int eval_move(struct node_t* move, short connectAmount);
short basic_bot_move(struct matrix_t* board, short player, struct settings_t* settings); // simply evaluate all the positions without going in depth - essentially depth 1
// short eval_pos_depth(struct matrix_t* board, short depth, short playerEval, short currentPlayer, short playerAmount, struct graphNode_t* graph, short connectionsForWin);
short eval_pos_breadth(struct matrix_t* board, short depth, short playerEval, short currentPlayer, short playerAmount, struct graphNode_t* current, struct graphNode_t* next, short connectionsForWin);
short eval_pos_depth(struct matrix_t* board, short depth, short playerEval, short currentPlayer, short playerAmount, short connectionsForWin);

// short bot_move(struct matrix_t* matrix, short player, short current_move, short max_moves, struct settings_t* settings, struct dict_t* colourDict);
// bool bot_can_play(struct matrix_t* matrix, short position);
// bool bot_winning_move(struct matrix_t* matrix, short player, short position, struct settings_t* settings);
// int negamax(struct matrix_t* matrix, short player, short current_move, short max_moves, struct settings_t* settings, int alpha, int beta);


/* -------------------------------------------------------------------------- */


/* FUNCTION DEFINITIONS */

short basic_bot_move(struct matrix_t* board, short player, struct settings_t* settings){

    struct node_t* newPiece;

    short bestColumn = -1;
    int bestEval = -1;

    int currentEval;

    for(short i = 0; i<board->columns; i++){
        newPiece = add_piece(board, player, i);

        if(newPiece == NULL){
            continue;
        }

        currentEval = eval_move(newPiece, settings->gameSettings.connectAmount);

        if(currentEval > bestEval){
            bestEval = currentEval;
            bestColumn = i;
        }

        newPiece->type = 0;

    }

    return bestColumn + 1;

}

int eval_move(struct node_t* move, short connectAmount){
    log_stderr(0, 0, "Checking for win");

    // get player
    short player = -1;
    int score = 0;

    // check if player is valid
    if(player == 0){
        // player is empty space
        log_stderr(0, 2, "Cannot check for win from empty node");
        return false;
    }

    struct node_t* node;
    short piece_count;


    // get score for current player

    // up and down case -> if both have the same node type

    // printf("%d \n", score);


    if((move->up != NULL && move->down != NULL) && (move->up->type == move->down->type) && move->up->type != 0){

        player = move->up->type;

        for(piece_count = 0, node = move; node->up != NULL && node->up->type == player; node = node->up, piece_count++);
        for(node = move; node->down != NULL && node->down->type == player; node = node->down, piece_count++);

        // how many nodes player connected
        if(player == move->type){

            // player won
            if(piece_count == connectAmount-1){ // -1 because start counting from 0
                return 10000;
            }

            // player didnt win -> add score based on how many connections he made

            score += pow(2.0, piece_count);
        }else{
            // insentivise it to play near opponents nodes
            score += pow(3.0, piece_count);
        }

    }else{
        // up case and down case separately

        // up case
        // even tho there is never anything above the most recent node, up is here if evaluating for previous move (althought that would be inaccurate)

        // get up type
        if(move->up != NULL){
            player = move->up->type;
        }

        // count pieces up
        piece_count = 0;
        for(node = move; node->up != NULL && node->up->type == player && player != 0; node = node->up, piece_count++);

        // add score

        // how many nodes player connected
        if(player == move->type){

            // player won
            if(piece_count == connectAmount-1){ // -1 because start counting from 0
                return 10000;
            }

            // player didnt win -> add score based on how many connections he made
            if(piece_count != 0){
                score += pow(2.0, piece_count);
            }

        }else{
            // insentivise it to play near opponents nodes
            if(piece_count != 0){
                score += pow(3.0, piece_count);
            }
        }

        // down case

        // get down type
        if(move->down != NULL){
            player = move->down->type;
        }

        // count pieces down
        piece_count = 0;
        for(node = move; node->down != NULL && node->down->type == player && player != 0; node = node->down, piece_count++);

        // add score

        // how many nodes player connected
        if(player == move->type){

            // player won
            if(piece_count == connectAmount-1){ // -1 because start counting from 0
                return 10000;
            }

            // player didnt win -> add score based on how many connections he made
            if(piece_count != 0){
                score += pow(2.0, piece_count);
            }
        }else{
            // insentivise it to play near opponents nodes
            if(piece_count != 0){
                score += pow(3.0, piece_count);
            }
        }
    }

    // printf("%d \n", score);

    if((move->left != NULL && move->right != NULL) && (move->left->type == move->right->type) && move->left->type != 0){

        player = move->left->type;

        // left and right case together if both are occupied by the same player

        for(piece_count = 0, node = move; node->left != NULL && node->left->type == player; node = node->left, piece_count++);
        for(node = move; node->right != NULL && node->right->type == player; node = node->right, piece_count++);

        if(player == move->type){

            // player won
            if(piece_count == connectAmount-1){ // -1 because start counting from 0
                return 10000;
            }

            // player didnt win -> add score based on how many connections he made
            if(piece_count != 0){
                score += pow(2.0, piece_count);
            }
        }else{
            // insentivise it to play near opponents nodes
            if(piece_count != 0){
                score += pow(3.0, piece_count);
            }
        }
    }else{
        // left and righ separately

        // left case

        // get left type
        if(move->left != NULL){
            player = move->left->type;
        }

        // count pieces left
        piece_count = 0;
        for(node = move; node->left != NULL && node->left->type == player && player != 0; node = node->left, piece_count++);

        // add score

        // how many nodes player connected
        if(player == move->type){

            // player won
            if(piece_count == connectAmount-1){ // -1 because start counting from 0
                return 10000;
            }

            // player didnt win -> add score based on how many connections he made

            if(piece_count != 0){
                score += pow(2.0, piece_count);
            }
        }else{
            // insentivise it to play near opponents nodes
            if(piece_count != 0){
                score += pow(3.0, piece_count);
            }
        }


        // right case

        // get right type
        if(move->right != NULL){
            player = move->right->type;
        }

        // count pieces to the right
        piece_count = 0;
        for(node = move; node->right != NULL && node->right->type == player && player != 0; node = node->right, piece_count++);

        // add score

        // how many nodes player connected
        if(player == move->type){

            // player won
            if(piece_count == connectAmount-1){ // -1 because start counting from 0
                return 10000;
            }

            // player didnt win -> add score based on how many connections he made

            if(piece_count != 0){
                score += pow(2.0, piece_count);
            }
        }else{
            // insentivise it to play near opponents nodes
            if(piece_count != 0){
                score += pow(3.0, piece_count);
            }

        }
    }

    // printf("%d \n", score);

    if( ( (move->up != NULL && move->left != NULL) && (move->down != NULL && move->right != NULL) ) && (move->up->left->type == move->down->right->type) && move->left->up->type != 0){

        player = move->up->left->type;

        // diagonal left right case together

        // diagonal left right case
        for(piece_count = 0, node = move; (node->left != NULL && node->left->up != NULL) && node->left->up->type == player; node = node->left->up, piece_count++);
        for(node = move; (node->right != NULL && node->right->down != NULL) && node->right->down->type == player; node = node->right->down, piece_count++);
    }else{
        // diagonal left right case separately


        // diagonal up left case

        if(move->left != NULL && move->left->up != NULL){
            player = move->left->up->type;
        }

        // count pieces diagonally (up left)
        piece_count = 0;
        for(node = move; (node->left != NULL && node->left->up != NULL) && node->left->up->type == player && player != 0; node = node->left->up, piece_count++);

        // how many nodes player connected
        // prefer diagonal connections because they are harder to intersept and can make traps
        if(player == move->type){

            // player won
            if(piece_count == connectAmount-1){ // -1 because start counting from 0
                return 10000;
            }

            // player didnt win -> add score based on how many connections he made

            if(piece_count != 0){
                score += pow(2.0, piece_count);
            }
        }else{
            // insentivise it to play near opponents nodes
            if(piece_count != 0){
                score += pow(3.0, piece_count);
            }
        }


        // diagonal right down case

        if(move->right != NULL && move->right->down != NULL){
            player = move->right->down->type;
        }

        // count pieces diagonally (down right)
        piece_count = 0;
        for(node = move; (node->right != NULL && node->right->down != NULL) && node->right->down->type == player && player != 0; node = node->right->down, piece_count++);

        // how many nodes player connected
        // prefer diagonal connections because they are harder to intersept and can make traps
        if(player == move->type){

            // player won
            if(piece_count == connectAmount-1){ // -1 because start counting from 0
                return 10000;
            }

            // player didnt win -> add score based on how many connections he made

            if(piece_count != 0){
                score += pow(2.0, piece_count);
            }
        }else{
            // insentivise it to play near opponents nodes
            if(piece_count != 0){
                score += pow(3.0, piece_count);
            }
        }
    }

    // printf("%d \n", score);

    if( ( (move->up != NULL && move->right != NULL) && (move->down != NULL && move->left != NULL) ) && (move->up->right->type == move->down->left->type) && move->down->left->type != 0){

        player = move->up->right->type;

        // try doing right up and left down together

        // diagonal right left case
        for(piece_count = 0, node = move; (node->right != NULL && node->right->up != NULL) && node->right->up->type == player; node = node->right->up, piece_count++);
        for(node = move; (node->left != NULL && node->left->down != NULL) && node->left->down->type == player; node = node->left->down, piece_count++);

        if(player == move->type){

            // player won
            if(piece_count == connectAmount-1){ // -1 because start counting from 0
                return 10000;
            }

            // player didnt win -> add score based on how many connections he made

            if(piece_count != 0){
                score += pow(2.0, piece_count);
            }
        }else{
            // insentivise it to play near opponents nodes
            if(piece_count != 0){
                score += pow(3.0, piece_count);
            }
        }
    }else{
        // do right up and left down separately

        // diagonal right up case

        if(move->right != NULL && move->right->up != NULL){
            player = move->right->up->type;
        }

        // count pieces diagonally (up right)
        piece_count = 0;
        for(node = move; (node->right != NULL && node->right->up != NULL) && node->right->up->type == player && player != 0; node = node->right->up, piece_count++);

        // how many nodes player connected
        // prefer diagonal connections because they are harder to intersept and can make traps
        if(player == move->type){

            // player won
            if(piece_count == connectAmount-1){ // -1 because start counting from 0
                return 10000;
            }

            // player didnt win -> add score based on how many connections he made

            if(piece_count != 0){
                score += pow(2.0, piece_count);
            }
        }else{
            // insentivise it to play near opponents nodes
            if(piece_count != 0){
                score += pow(3.0, piece_count);
            }
        }


        // diagonal left down case

        if(move->left != NULL && move->left->down != NULL){
            player = move->left->down->type;
        }

        // count pieces diagonally (down left)
        piece_count = 0;
        for(node = move; (node->left != NULL && node->left->down != NULL) && node->left->down->type == player && player != 0; node = node->left->down, piece_count++);

        // how many nodes player connected
        // prefer diagonal connections because they are harder to intersept and can make traps
        if(player == move->type){

            // player won
            if(piece_count == connectAmount-1){ // -1 because start counting from 0
                return 10000;
            }

            // player didnt win -> add score based on how many connections he made

            if(piece_count != 0){
                score += pow(2.0, piece_count);
            }
        }else{
            // insentivise it to play near opponents nodes
            if(piece_count != 0){
                score += pow(3.0, piece_count);
            }
        }
    }


    return score;
}

short eval_pos_depth(struct matrix_t* board, short depth, short playerEval, short currentPlayer, short playerAmount, short connectionsForWin){

    // exit when depth is reached
    if(depth == 0){
        return 0;
    }else{
        // try adding a piece (in every column)
        for(short i = 0; i< board->columns; i++){

            struct node_t* newPiece = add_piece(board, currentPlayer, i);

            // if column is full
            if(newPiece == NULL){
                continue;
            }

            // check if new piece is a win
            if(check_win(newPiece, connectionsForWin, 0)){
                if(playerEval == currentPlayer){
                    return 1;
                }
            }
        }
    }

}

// short eval_pos_breadth(struct matrix_t* board, short depth, short playerEval, short currentPlayer, short playerAmount, struct queue_t* current, struct queue_t* next, short connectionsForWin){
//
//     /*
//     q node has both value and position params
//
//     value is the current column
//     position is the originating column (parentmost)
//
//     Only issue, but a major one is that I add pieces but dont remove them
//
//
//     */
//
//     struct qNode_t* currentNode = current->front;
//
//
//     if(depth == 0){
//
//         // return random column from current
//         short randomNode = rand() % current->size;
//
//         for(short i = 1; i < randomNode; i++){
//             currentNode = currentNode->next;
//         }
//
//         return currentNode->originalPosition;
//     }
//
//     struct node_t* newPiece;
//
//
//     for(short i = 0; i < current->size; i++, currentNode = currentNode->next){
//
//         newPiece = add_piece(board, currentPlayer, currentNode->currentPosition);
//
//         // check if columns are full
//         if(newPiece == NULL){
//             // skip column and remove frome current
//             dequeue(current);
//             continue;
//         }
//
//         currentNode->rowHistory[currentNode->historySize] = newPiece->row;
//         currentNode->columnHistory[currentNode->historySize] = newPiece->column;
//
//         // check if move is winning
//         if(check_win(newPiece, connectionsForWin, 0)){
//             // winning for bot (whoose turn it is)
//             if(currentPlayer == playerEval){
//                 currentNode->score = 1;
//                 return currentNode->originalPosition;
//             }else{ // winning for other player/bot
//                 dequeue(current);
//                 continue;
//             }
//         }else{
//             currentNode->score = 0;
//         }
//
//         // add to next
//         for(short b = 0; b<board->columns; b++){
//             enqueue(next, b);
//             next->front->originalPosition = currentNode->currentPosition;
//             memcpy(next->front->columnHistory, currentNode->columnHistory, sizeof(short)*currentNode->historySize+1);
//             next->front->columnHistory[next->front->historySize] = currentNode->currentPosition;
//             next->front->rowHistory[next->front->historySize++] = currentNode->currentPosition;
//         }
//
//         dequeue(current);
//     }
//
//     return eval_pos_breadth(board, depth-1, playerEval, 1 + (currentPlayer >= playerAmount ? 0 : currentPlayer), playerAmount, next, current, connectionsForWin);
//
// }

// short eval_pos_depth(struct matrix_t* board, short depth, short playerEval, short currentPlayer, short playerAmount, struct graphNode_t* graph, short connectionsForWin){
//
//     // log_stderr(0, 0, "Evaluating position");
//     struct node_t* newPiece = NULL;
//     short eval = 0;
//
//     // exit on depth
//     if(depth == 0 || graph->score != 0){
//         return eval;
//     }else{
//
//         // iterate over all possible positions
//         for(short i = 0; i < board->columns; i++){
//             // try inserting new piece
//             newPiece = add_piece(board, currentPlayer, i);
//
//             // check if columns are full
//             if(newPiece == NULL){
//                 // skip column
//                 continue;
//             }
//
//
//
//
//             // check if current insertion is a win/loss for the player getting evaluated
//             if(check_win(newPiece, connectionsForWin, 0)){
//                 if(currentPlayer == playerEval){
//                     eval = 1;
//                     add_graphNode_connection(graph, i, 1, 0);
//                 }else{
//                     eval = -1;
//                     add_graphNode_connection(graph, i, -1, 0);
//                 }
//             }else{
//                 // neither win or loss -> unknown/draw
//                 if(graph->connections != graph->activeConnections){
//                     add_graphNode_connection(graph, i, 0, board->columns);
//                 }
//             }
//             printf("\n\n");
//             basic_print_matrix(board);
//             print_graphNode(graph, depth);
//             getchar();
//             // go deeper
//
//             graph->score = eval_pos_depth(board, depth-1, playerEval, 1 + (currentPlayer >= playerAmount ? 0 : currentPlayer), playerAmount, graph->connectionArr[i], connectionsForWin);
//
//
//
//             // when going back remove the already placed pieces
//             newPiece->type = 0;
//
//         }
//     }
//
//     if(newPiece != NULL){
//         newPiece->type = 0;
//     }
//     return eval;
//
// }

short bot_moves(struct matrix_t* board, short player, struct settings_t* settings){

    log_stderr(0, 0, "Making a bot move");
    //
    // struct graph_t* graph = init_graph(board->columns);
    // struct node_t* newPiece;
    //
    // for(short i = 0; i< board->columns; i++){
    //
    //     newPiece = add_piece(board, player, i);
    //
    //     if(newPiece == NULL){
    //         continue;
    //     }
    //
    //     if(check_win(newPiece, settings->gameSettings.connectAmount, 0)){
    //         return i;
    //     }
    //     add_graph_branch(graph, i, 0, board->columns);
    //     eval_pos_breadth(board, settings->gameSettings.botDepth-1, player, 1 + (player >= settings->gameSettings.playerAmount ? 0 : player), settings->gameSettings.playerAmount, graph->branchArr[graph->activeBranches-1], settings->gameSettings.connectAmount);
    //     newPiece->type = 0;
    //
    // }
    //
    // short highestScoreColumn = get_highest_score_branch(graph)->value;
    //
    // // free_graph(graph);
    //
    // print_graph(graph);
    // return highestScoreColumn+1;
}

//
// short eval_pos_depth(struct matrix_t* board, short depth, short eval, short player, short currPlayer, short playerAmount, short connectAmount){
//
//     log_stderr(0, 0, "Evaluating position");
//     /*
//     player stores the original player who this func is evaluationg for
//     currPlayer is the current player that gets his moves evaluated
//     */
//
//     struct node_t* newPiece = NULL;
//
//     // stop evaluating when depth is reached
//     if(depth == 0){
//         return eval;
//     }else{
//         // eval moves for every column
//
//         for(short i = 0; i<board->columns; i++){
//
//             newPiece = add_piece(board, currPlayer, i);
//
//             // check if move in column i would lead to win/lose
//             if(newPiece == NULL){
//                 log_stderr(0, 0, "Cannot add piece -> column is full");
//                 continue;
//             }
//
//             // check if move is a win
//             if(check_win(newPiece, connectAmount, 0)){
//                 if(player == currPlayer){
//                     // if original currPlayer can win
//                     eval = 1;
//                 }else{
//                     // if someone else wins
//                     eval = -1;
//                 }
//             }
//
//             // basic_print_matrix(board);
//             // printf("\n %hd \n", eval);
//
//             if(eval != 0){
//                 break;
//             }
//
//             eval = eval_pos_depth(board, depth-1, eval, player, 1 + (currPlayer >= playerAmount ? 0 : currPlayer), playerAmount, connectAmount);
//
//             // switch piece type back
//             newPiece->type = 0;
//         }
//
//     }
//
//     if(newPiece != NULL){
//         newPiece->type = 0;
//     }
//
//     return eval;
//
// }

void game_loop(struct settings_t* settings, struct stats_t* stats, struct dict_t* colourDict)
{

    log_stderr(0, 1, "Starting game loop");

    // create matrix

    struct matrix_t* matrix = create_matrix(settings->gameSettings.boardRows, settings->gameSettings.boardColumns);
        if(matrix == NULL){
            log_stderr(0, 3, "Failed creating matrix");
            return;
        }else{
            log_stderr(0, 0, "Successfully created matrix");
        }

    // get filenames

    char log_name[settings->fileSettings.logFileNameLen + strlen("../res/")];
    sprintf(log_name, "../res/%s", settings->fileSettings.logFileName);
    // strcat(log_name, settings->fileSettings.logFileName);
    char stats_name[settings->fileSettings.statsFileNameLen + strlen("../res/bin/")];
    sprintf(stats_name, "../res/bin/%s", settings->fileSettings.statsFileName);


    short player = 1;
    short position = 0;
    struct node_t* newNode;

    const short max_moves = matrix->rows * matrix->columns;

    short moves[max_moves];
    memset(moves, 0, max_moves*sizeof(short));

    stats->total_games++;

    for (int x = 0; x < settings->gameSettings.playerAmount; x++){
		    stats->player[x].games++;
	  }

    // clock_t start, stop;
    time_t start, stop;
    start = time(NULL);

	// Get player input - Done
	// Verify input - Done
	// Add piece - Done
	// Log move - Done
	// Display matrix - Done
	// Check win - Done
    //start = clock();

    print_matrix(matrix, settings, colourDict);

	for (short i = 0; 1; player = 1 + (player >= settings->gameSettings.playerAmount ? 0 : player), i++)
	{
        while (1){
            printf("%d/%d \n", i, max_moves);
			      printf("Player %hd: ", player);

            if(settings->gameSettings.againstBot == 0){
                log_stderr(0, 0, "Ask player for column pick");
                    while(get_short_from_char(&position, 1, matrix->columns, "Invalid position!\nchoose column: ") != true){
                    // msg repeating because if get_short exits from esc it won't say any msg
                        printf("Invalid position!\nchoose column: ");
                    };
            }else{

              if(player == 1){
                  log_stderr(0, 0, "Ask player for column pick");
                      while(get_short_from_char(&position, 1, matrix->columns, "Invalid position!\nchoose column: ") != true){
                      // msg repeating because if get_short exits from esc it won't say any msg
                          printf("Invalid position!\nchoose column: ");
                      };
              }else{
                  position = basic_bot_move(matrix, player, settings);
              }
            }

            position--;

            newNode = add_piece(matrix, player, position);

            // position was invalid (usually adding piece in full column)
            if(newNode == NULL){
                continue; // loop again to get valid input
            }

  			break;
  		}

		moves[i] = position+1;
		stats->total_moves++;
		stats->player[player-1].moves++;

    print_matrix(matrix, settings, colourDict);
    printf("\n Move Score -> %d\n", eval_move(newNode, settings->gameSettings.connectAmount));

        // win condition
    if(check_win(newNode, settings->gameSettings.connectAmount, 1)){
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

  stop = time(NULL);

	long long duration = stop - start;
  stats->total_playtime += duration;
  for (int x = 0; x < settings->gameSettings.playerAmount; stats->player[x++].playtime += duration);

	// log_moves(matrix, moves, max_moves, log_name);
  printf("Press any key to continue: ");
  getchar();
  // log and free after getchar so that logging isnt shown on screen
  log_moves(matrix, moves, max_moves, log_name, settings->playerSettings);
  write_stats_file(stats, stats_name);
  free_matrix(matrix);
	return;
}

struct node_t* add_piece(struct matrix_t* matrix, short player, short position)
{

    log_stderr(0, 0, "Adding piece");

	if (position < 0 || position >= matrix->columns){
        // shouldnt be possible with input check
        log_stderr(0, 2, "Position out of bounds");
		return NULL;
	}

	struct node_t* node = get_node_by_cords(matrix, 0, position);


	for(; node != NULL && node->type != 0; node = node->up);

	if (node == NULL){
        log_stderr(0, 2, "Column already full, cannot add piece");
        return NULL;
	}
	else{
		node->type = player;
	}

	return node;
}

// O(connectAmount) 8*connectAmount to be exact

bool check_win(struct node_t* start, short connectAmount, bool colour){

    /*
    check win from a current node (all its directions)
    the most recent node should be used as start
    */

    log_stderr(0, 0, "Checking for win");

    // get player
    short player = start->type;

    // check if player is valid
    if(player == 0){
        // player is empty space
        log_stderr(0, 2, "Cannot check for win from empty node");
        return false;
    }

    struct node_t* node;
    short piece_count;
    bool checkWin = false;

    // up and down case
    for(piece_count = 1, node = start; node->up != NULL && node->up->type == player; node = node->up, piece_count++);
    for(node = start; node->down != NULL && node->down->type == player; node = node->down, piece_count++);
    if(piece_count >= connectAmount){
        if(colour == false){
            return true;
        }else{
            // change win highlight

            node->winHiglight = true; // current node

            // linked nodes
            for(; node->up != NULL && node->up->type == player; node = node->up, node->winHiglight = true);
            // dont return straight away to colour double connections
            checkWin = true;
        }

    }

    // left and right case
    for(piece_count = 1, node = start; node->left != NULL && node->left->type == player; node = node->left, piece_count++);
    for(node = start; node->right != NULL && node->right->type == player; node = node->right, piece_count++);
    if(piece_count >= connectAmount){
        if(colour == false){
            return true;
        }else{
            // change win higlight
            node->winHiglight = true; // current node
            for(; node->left != NULL && node->left->type == player; node = node->left, node->winHiglight = true);
            checkWin = true;
        }
    }

    // diagonal left right case
    for(piece_count = 1, node = start; (node->left != NULL && node->left->up != NULL) && node->left->up->type == player; node = node->left->up, piece_count++);
    for(node = start; (node->right != NULL && node->right->down != NULL) && node->right->down->type == player; node = node->right->down, piece_count++);
    if(piece_count >= connectAmount){
        // change win higlight
        if(colour == false){
            return true;
        }else{
            node->winHiglight = true; // current node
            for(; (node->left != NULL && node->left->up != NULL) && node->left->up->type == player; node = node->left->up, node->winHiglight = true);
            checkWin = true;
        }
    }

    // diagonal right left case
    for(piece_count = 1, node = start; (node->right != NULL && node->right->up != NULL) && node->right->up->type == player; node = node->right->up, piece_count++);
    for(node = start; (node->left != NULL && node->left->down != NULL) && node->left->down->type == player; node = node->left->down, piece_count++);
    if(piece_count >= connectAmount){
        // change win higlight
        if(colour == false){
            return true;
        }else{
            node->winHiglight = true; // current node
            for(; (node->right != NULL && node->right->up != NULL) && node->right->up->type == player; node = node->right->up, node->winHiglight = true);
            checkWin = true;
        }
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


//
// short bot_move(struct matrix_t* matrix, short player, short current_move, short max_moves, struct settings_t* settings, struct dict_t* colourDict)
// {
// 	int scores[matrix->columns];
//
// 	for (int x = 0; x < matrix->columns; x++)
// 	{
//     /*
//       NOTE: You cant be creating matrixes for every column o every bot move - too wasteful
//     */
// 		if (bot_can_play(matrix, x))
// 		{
// 			printf("%d ", x);
// 			struct matrix_t* bot_matrix = clone_matrix(matrix);
// 			add_piece(bot_matrix, player, x);
// 			scores[x] = -negamax(bot_matrix, player, current_move + 1, max_moves, settings, -10000, 10000);
// 			free_matrix(bot_matrix);
// 		}
// 		else
// 		{
// 			scores[x] = -matrix->rows * matrix->columns - 1;
// 		}
// 	}
//
// 	int highest_score_x = 0;
// 	for (int x = 1, highest_score = scores[0]; x < matrix->columns; x++)
// 	{
// 		if (highest_score < scores[x])
// 		{
// 			highest_score = scores[x];
// 			highest_score_x = x;
// 		}
// 	}
//
// 	// printf("\nScores: ");
// 	// for (int x = 0; x < matrix->columns; x++)
// 	// {
// 	// 	printf("%d ", scores[x]);
// 	// }
// 	// printf("\n");
//
// 	return highest_score_x;
// }
//
// // Can the position be played
// bool bot_can_play(struct matrix_t* matrix, short position)
// {
// 	if (position < 0 || position >= matrix->columns)
// 	{
// 		return false;
// 	}
//
// 	return get_node_by_cords(matrix, matrix->rows-1, position)->type == 0;
// }
//
// // Would putting a piece in that position be a win
// bool bot_winning_move(struct matrix_t* matrix, short player, short position, struct settings_t* settings)
// {
// 	struct node_t* node = get_node_by_cords(matrix, 0, position);
//
// 	for(; node != NULL && node->type != 0; node = node->up);
//
// 	node->type = player;
//
// 	bool is_winning = check_win(node, settings->gameSettings.connectAmount);
//
// 	node->type = 0;
//
// 	return is_winning;
// }
//
// int negamax(struct matrix_t* matrix, short player, short current_move, short max_moves, struct settings_t* settings, int alpha, int beta)
// {
// 	if (current_move >= max_moves)
// 	{
// 		return 0;
// 	}
//
// 	for (int x = 0; x < matrix->columns; x++)
// 	{
// 		if (bot_can_play(matrix, x) && bot_winning_move(matrix, player, x, settings))
// 		{
// 			return (matrix->rows * matrix->columns + 1 - current_move) / 2;
// 		}
// 	}
//
// 	int max_score = (matrix->rows * matrix->columns - 1 - current_move) / 2;
// 	if (beta > max_score)
// 	{
// 		beta = max_score;
// 		if (alpha >= beta)
// 		{
// 			return beta;
// 		}
// 	}
//
// 	for (int x = 0; x < matrix->columns; x++)
// 	{
// 		if (bot_can_play(matrix, x))
// 		{
// 			struct matrix_t* next_matrix = clone_matrix(matrix);
// 			short next_player = 1 + (player >= settings->gameSettings.playerAmount ? 0 : player);
//
// 			add_piece(next_matrix, next_player, x);
// 			int score = -negamax(next_matrix, next_player, current_move + 1, max_moves, settings, -alpha, -beta);
//
// 			free_matrix(next_matrix);
//
// 			if (score >= beta)
// 			{
// 				return score;
// 			}
// 			if (score > alpha)
// 			{
// 				alpha = score;
// 			}
// 		}
// 	}
//
// 	return alpha;
// }
//
// /* -------------------------------------------------------------------------- */
