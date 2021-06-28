/* IMPORTED LIBRARIES */

#include <time.h>

#include "./libs/game/logic/game.h"

/* -------------------------------------------------------------------------- */


/* FUNCTION DECLARATIONS */

void init(); // starts main menu and prepares all needed vars
void display_main_menu(struct settings_t* settings, struct dict_t* colourDict, struct stats_t* stats);
void quit(struct settings_t* settings, struct dict_t* colourDict, struct stats_t* stats); // frees all rescources and quits  NOTE: add stats when done

/* -------------------------------------------------------------------------- */


/* MAIN */

int main(int argc, char const *argv[]) {

    // init
    init();

    return 0;
}

/* -------------------------------------------------------------------------- */


/* FUNCTION DEFINITIONS */

void quit(struct settings_t* settings, struct dict_t* colourDict, struct stats_t* stats){

    log_stderr(0, 1, "Freeing rescources");

    // free dictionary
    free_dict(colourDict);

    // free settings
    free_settings(settings);

    free_stats(stats);

    system("clear");

    log_stderr(0, 1, "Quitting program");

    return;
}

void init(){

    // debug mode always active on launch
    log_stderr(1, 1, "Launching program");

    // get colour dict
    log_stderr(0, 1, "Initializing colour dictionary");

    struct dict_t* colourDict = init_colour_dict();

    // check if dict failed
    if(colourDict == NULL){
        log_stderr(0, 3, "Failed initializing colour dictionary");
        quit(NULL, colourDict, NULL);
        return;
    }

    // get settings

    log_stderr(0, 1, "Initializing settings");
    bool settings_success = false;

    struct settings_t* settings = init_settings("settings.bin", colourDict, &settings_success);


    // check if settings failed
    if(settings_success == false){
        log_stderr(0, 3, "Failed initializing settings");
        quit(settings, colourDict, NULL);
        return;
    }

    // get stats
    bool stats_success = false;
    struct stats_t* stats = init_stats("stats.bin", &stats_success);

    if(stats_success == false){
      quit(settings, colourDict, stats);
    }

    log_stderr(!settings->gameSettings.debugMode, 0, "Changing debug mode from settings"); // return back to debugmode from settings

    // set rand seed
    log_stderr(0, 0, "Generating seed");
    srand(time(NULL));

    // launch main menu
    display_main_menu(settings, colourDict, stats);


    // after main menu quit -> free all rescources
    quit(settings, colourDict, stats);
}

void display_main_menu(struct settings_t* settings, struct dict_t* colourDict, struct stats_t* stats){

    log_stderr(0, 1, "Displaying main menu");


    short menuNO;
    bool exitCheck;

    log_stderr(0, 0, "Fetching colours from colour dict");
    const char *yellow, *def, *flashing, *white, *heavy;
    yellow = binary_search_dict('Y', colourDict);
    def = binary_search_dict('D', colourDict);
    flashing = binary_search_dict('F', colourDict);
    white = binary_search_dict('W', colourDict);
    heavy = binary_search_dict('H', colourDict);
    char errmsg[] = {" \n Invalid input! Try again\n go to"};

    do{
        log_stderr(0, 0, "Printing main menu");
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
            log_stderr(0, 1, "Exiting program");
            printf("Exiting program!\n");
            break; // OPT: return instead of break?
        }

        if(menuNO == 1){ // start new game

        game_loop(settings, stats, colourDict);
        }else if(menuNO == 2){ // show stats menu

            display_stats_menu(stats, colourDict);
        }else if(menuNO == 3){ // show settings menu

            display_settings_menu(settings, colourDict);
        }


    }while(1);

}

/* -------------------------------------------------------------------------- */
