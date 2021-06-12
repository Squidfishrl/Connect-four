/* IMPORTED LIBRARIES */

#include <stdio.h>
#include <stdlib.h>
#include "game.h"
// also include game logic once its done

/* -------------------------------------------------------------------------- */


/* FUNCTION DECLARATIONS */

void init(); // TODO: check if files such as settings and stats exist. if not create them
void display_main_menu();
void display_settings_menu();

/* -------------------------------------------------------------------------- */


/* MAIN */

int main(int argc, char const *argv[]) {

    // init
    display_main_menu();

    return 0;
}

/* -------------------------------------------------------------------------- */


/* FUNCTION DEFINITIONS */

void display_settings_menu(){
    /*
        TODO: read settings file - display settings in a similar way to main menu
        TODO: select settings and be able to change values (with error handling)
        TODO: also add 0) Back -> essentially does: return;
    */
}

void display_main_menu(){



    int optionPick;

    do{

        system("clear");


        printf("\033[0;93m \033[1m CONNECT FOUR \033[0m ");
        printf("\n\n");
        printf(" \033[0;5m-\033[0m\033[0;97m 1) NEW GAME \033[0m \n");
        printf(" \033[0;5m-\033[0m\033[0;97m 2) STATS \033[0m \n");
        printf(" \033[0;5m-\033[0m\033[0;97m 3) SETTINGS \033[0m \n");
        printf(" \033[0;5m-\033[0m\033[0;97m 4) QUIT \033[0m \n");

        printf(" \n go to: ");


        scanf("%d", &optionPick);

        switch(optionPick){

            case 1: // new game
                struct matrix_t* matrix = create_matrix(20, 7);
                char str[] = "log.log";
                game_loop(matrix, str);
                break;
            case 2: // stats
                // TODO: once game loop is done, track different stats during game loop (add a struct for it and save as binary file)
                // TODO: this displays them
                break;
            case 3: // settings
                // display_settings_menu();
                break;
            case 4: // quit
                printf("Exiting program!");
                break;
            default:
                printf("Error - invalid option \n\n");
                break; // not needed but whatever
        }

    }while(optionPick != 4);




}

/* -------------------------------------------------------------------------- */
