/* IMPORTED LIBRARIES */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* -------------------------------------------------------------------------- */


/* STRUCTURE DEFINITIONS */

struct player_t{

    // TODO: in board.h change int type to player_t* type.. make it compatible
    short number;
    char* colour;
    char symbol;
};

struct settings_t{

    short boardRows;
    short boardColumns;

    const short maxRows = 20;
    const short maxColumns = 40;

    bool againstBot; // TODO: add bot option, simple algorythm O(columns^depth)
    short botDepth;

    bool multiplayerMode; // 0 -> local; 1 -> TODO: socket connection

    short playerAmount; // rewrite to feature more players
    struct player_t** playerSettings; // dynamic array of *players ive entered settings for
    char* winHighlightColour;

    /* OPT: if find a way to change terminal font size add settings for that
    setting could be read by a bash script if not possible in c
    */

    char* logFileName; // should be private and always log but rules disagree..
    char* settingsFileName; // following that logic
    char* statsFileName;
};

/* -------------------------------------------------------------------------- */


/* FUNCTION DECLARATIONS */

struct settings_t* init_settings(char* fileName);
struct settings_t* define_settings(char* fileName); // create default settings
bool change_setting(struct settings_t* settings, short settingNO); // NOTE: if editing player settings and adding a setting for a new player, check if the player is actually new or not
bool write_settings(char* fileName, struct settings_t* settings);
void display_settings_menu(struct settings_t* settings); // could add filename depending what I do in it
void free_settings(struct settings_t* settings);

/* -------------------------------------------------------------------------- */


/* FUNCTION DEFINITIONS */

struct settings_t* init_settings(char* fileName){

    struct settings_t* settings = (struct settings_t*)malloc(sizeof(struct settings_t));
    FILE* settingsFile;

    // Try to open file
    if((settingsFile = fopen(fileName, "r"))){
        // read from .bin to a struct

    } // File doesn't exist; try to create it
    else if((settingsFile = fopen(fileName, "w+"))){
        // default values
        settings->boardRows = 6;
        settings->boardColumns = 7;
        settings->againstBot = false;
        settings->botDepth = 5; // O(40^5) worst case
        settings->multiplayerMode = false;
        settings->playerAmount = 2;

        settings->playerSettings = (struct player_t**)malloc(settings->playerAmount * sizeof(struct player_t*));

        // TODO: add a seperate file for console colors -> like a hashmap with functions

        // player 1
        settings->playerSettings[0]->number = 1;
        settings->playerSettings[0]->colour = "RED";
        settings->playerSettings[0]->symbol = 'X';

        // player 2
        settings->playerSettings[0]->number = 2;
        settings->playerSettings[0]->colour = "BLUE";
        settings->playerSettings[0]->symbol = 'O';

        settings->winHighlightColour = "Green flashing";

        settings->logFileName = "log.txt";
        settings->settingsFileName = "settings.bin";
        settings->statsFileName = "stats.bin";

    } // Can't create file
    else{
        free(settings);
        printf("Failed to create or open settings file!\n");
        return NULL;
    }
}

/* -------------------------------------------------------------------------- */
