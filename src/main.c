/* IMPORTED LIBRARIES */

#include <time.h>

#include "./libs/game/logic/game.h"

/* -------------------------------------------------------------------------- */


/* FUNCTION DECLARATIONS */

void init(); // starts main menu and prepares all needed vars
void display_main_menu(struct settings_t* settings, struct dict_t* colourDict);
void quit(struct settings_t* settings, struct dict_t* colourDict); // frees all rescources and quits  NOTE: add stats when done

/* -------------------------------------------------------------------------- */


/* MAIN */

int main(int argc, char const *argv[]) {

    // init
    init();

    return 0;
}

/* -------------------------------------------------------------------------- */


/* FUNCTION DEFINITIONS */

void quit(struct settings_t* settings, struct dict_t* colourDict){

    log_stderr(0, 1, "Freeing settings");
    free_settings(settings);

    log_stderr(0, 1, "Freeing colour dictionary");
    free_dict(colourDict);

    return;
}

void init(){

    log_stderr(0, 1, "Launching program");

    // get colour dict
    log_stderr(0, 1, "Initializing colour dictionary");
    struct dict_t* colourDict = init_colour_dict();
    // get settings
    log_stderr(0, 1, "Initializing settings");
    struct settings_t* settings = init_settings("settings.bin", colourDict);
    // get stats
    // TODO:

    // set rand seed
    srand(time(NULL));

    // launch main menu
    log_stderr(0, 1, "Displaying main menu");
    display_main_menu(settings, colourDict);

    // after main menu quit -> free all rescources
    quit(settings, colourDict);
}

void display_main_menu(struct settings_t* settings, struct dict_t* colourDict){

    short menuNO;
    bool exitCheck;

    const char *yellow, *def, *flashing, *white, *heavy;
    yellow = binary_search_dict('Y', colourDict);
    def = binary_search_dict('D', colourDict);
    flashing = binary_search_dict('F', colourDict);
    white = binary_search_dict('W', colourDict);
    heavy = binary_search_dict('H', colourDict);
    char errmsg[] = {" \n Invalid input! Try again\n go to"};

    do{

        system("clear");


        printf("%s%s CONNECT FOUR %s", heavy, yellow, def);
        printf("%s (main-menu) %s", yellow, def);
        printf("\n\n");
        printf(" %s-%s%s 1) NEW GAME %s \n", flashing, def, white, def);
        printf(" %s-%s%s 2) STATS %s \n", flashing, def, white, def);
        printf(" %s-%s%s 3) SETTINGS %s \n", flashing, def, white, def);
        printf(" %s-%s%s 4) QUIT %s \n", flashing, def, white, def);

        // not in do while loop to allow error msg
        printf(" \n go to: ");
        // get input and on esc exit program
        exitCheck = !get_short(&menuNO, 1, 4, errmsg);

        if(exitCheck == true || menuNO == 4){
            printf("Exiting program!\n");
            break; // same as return cuz nothing after while(1)
        }

        if(menuNO == 1){ // start new game
            log_stderr(0, 1, "Launching game");
            game_loop(settings, colourDict);
        }else if(menuNO == 2){ // show stats menu
            log_stderr(0, 1, "Displaying stats menu");
        }else if(menuNO == 3){ // show settings menu
            log_stderr(0, 1, "Displaying settings menu");
            display_settings_menu(settings, colourDict);
        }


    }while(1);

}

/* -------------------------------------------------------------------------- */
