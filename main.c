/* IMPORTED LIBRARIES */

#include "game.h"

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
    free_settings(settings);
    free_dict(colourDict);
}

void init(){

    // get colour dict
    struct dict_t* colourDict = init_colour_dict();
    // get settings
    struct settings_t* settings = init_settings("settings.bin", colourDict);
    // get stats
    // TODO:


    // launch main menu
    display_main_menu(settings, colourDict);

    // after main menu quit -> free all rescources
    quit(settings, colourDict);
}

void display_main_menu(struct settings_t* settings, struct dict_t* colourDict){

    short menuNO;

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
        get_short(&menuNO, 1, 4, errmsg);

        switch(menuNO){

            case 1: // new game;
                game_loop(settings, colourDict);
                break;
            case 2: // stats
                // TODO: once game loop is done, track different stats during game loop (add a struct for it and save as binary file)
                // TODO: this displays them
                break;
            case 3: // settings
                display_settings_menu(settings, colourDict);
                break;
            case 4: // quit
                printf("Exiting program!\n");
                break;
            default:
                // shouldnt be possible and if it is printf wouldnt be seen
                printf("Error - invalid option \n\n");
                break;
        }

    }while(menuNO != 4);

}

/* -------------------------------------------------------------------------- */
