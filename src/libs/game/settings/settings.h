/* IMPORTED LIBRARIES */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#include "../../general/get_stdin.h"

/* -------------------------------------------------------------------------- */


/* STRUCTURE DEFINITIONS */

struct gameSettings_t{

    short boardRows;
    short boardColumns;

    const short maxRows; // 20
    const short maxColumns; // 40

    short connectAmount; // how many nodes to connect for a win

    bool againstBot; // TODO: add bot option, simple algorithm (minmax) O(columns^depth)
    short botDepth;

    // bool multiplayer; // f -> local; t -> TODO: socket connection

    short playerAmount; // rewrite to feature more players
    const short maxPlayers;

    char winHighlightColour;

    bool debugMode;
    /* OPT: if find a way to change terminal font size add settings for that
    setting could be read by a bash script if not possible in c
    */

};

struct fileSettings_t{

    uint8_t logFileNameLen;
    char logFileName[31]; // should be private and always log but rules disagree..

    uint8_t settingsFileNameLen;
    char settingsFileName[31]; // following that logic

    uint8_t statsFileNameLen;
    char statsFileName[31];
};

struct player_t{

    // TODO: in board.h change int type to player_t* type.. make it compatible
    uint8_t number;
    char colour;
    char symbol;

    uint8_t colourCodeLen; // even tho its a str (ending in '\0') fread doesn't care
    char colourCode[6]; // would be nice if it could be a pointer but I keep getting fread errors when it is
};

struct playerSettings_t{
    uint8_t playerArrSize; // private
    struct player_t** playerSettings; // dynamic array of *players ive entered settings for
};

struct settings_t{
    struct gameSettings_t gameSettings;
    struct playerSettings_t* playerSettings;
    struct fileSettings_t fileSettings;
};

struct displaySettings_t{ // TODO: eventually write/read when im feeling like dereferencing pointers.. could also add to gameSettings_t
    char* name;
    char* description;
    void* value;
    const short minValue;
    const short maxValue;
};

/* -------------------------------------------------------------------------- */


/* FUNCTION DECLARATIONS */

struct settings_t*  init_settings(char* fileName, struct dict_t* colourDict, bool* success);
struct settings_t* define_settings(struct dict_t* colourDict); // create default settings
bool read_settings(char* fileName, struct settings_t* settings);
bool write_settings(char* fileName, struct settings_t* settings);
void free_settings(struct settings_t* settings);
void add_player(struct playerSettings_t* settings, struct dict_t* colourDict);

void display_settings_menu(struct settings_t* settings, struct dict_t* colourDict); // NOTE: could add filename depending what I do in it
bool display_game_settings_menu(struct settings_t* settings, struct dict_t* colourDict); // returns if changes were made
bool display_player_settings_menu(struct settings_t* settings, struct dict_t* colourDict);
bool display_file_settings_menu(struct settings_t* settings, struct dict_t* colourDict);

/* -------------------------------------------------------------------------- */


/* FUNCTION DEFINITIONS */

// TODO: add return value (if it fails or not)
void add_player(struct playerSettings_t* settings, struct dict_t* colourDict){

    log_stderr(0, 0, "Adding a new player");
    // allocate space for a new player

    settings->playerSettings = (struct player_t**)realloc(settings->playerSettings, ++settings->playerArrSize * sizeof(struct player_t*) );
    if(settings->playerSettings == NULL){
        log_stderr(0, 3, "Reallocating player settings failed, causing a memory leak");
        return;
    }else{
        log_stderr(0, 0, "Successfully reallocated player settings");
    }
    // allocate new player
    settings->playerSettings[settings->playerArrSize-1] = (struct player_t*)malloc(sizeof(struct player_t));
    // check if malloc failed
    if(settings->playerSettings[settings->playerArrSize-1] == NULL){
        log_stderr(0, 3, "Allocating memory for player failed");
        return;
    }else{
        log_stderr(0, 0, "Successfully allocated memory player settings");
    }
    //assign number
    log_stderr(0, 0, "Assigning player number");
    settings->playerSettings[settings->playerArrSize-1]->number = settings->playerArrSize;

    /* assign random colour */

    // colour key
    log_stderr(0, 0, "Assigning player colour");
    settings->playerSettings[settings->playerArrSize-1]->colour = fetch_random_colour(colourDict);

    //colour code
    log_stderr(0, 0, "Assigning player colour code");
    strcpy(settings->playerSettings[settings->playerArrSize-1]->colourCode, binary_search_dict(settings->playerSettings[settings->playerArrSize-1]->colour, colourDict));

    // colour code len
    log_stderr(0, 0, "Assigning player colour code len");
    settings->playerSettings[settings->playerArrSize-1]->colourCodeLen = strlen(settings->playerSettings[settings->playerArrSize-1]->colourCode);

    // symbol
    log_stderr(0, 0, "Assigning player colour code len");
    settings->playerSettings[settings->playerArrSize-1]->symbol = '1' + settings->playerArrSize - 1;
}

void free_settings(struct settings_t* settings){

    log_stderr(0, 1, "Freeing settings");
    char logMsg[30];

    // check if settings are null
    if(settings != NULL){

        // check if player settings are null
        if(settings->playerSettings != NULL){

            // check if player arr is null
            if(settings->playerSettings->playerSettings != NULL){

                // free player settings[] players
                for(short i = 0; i < settings->playerSettings->playerArrSize; i++){

                    // check if player is NULL
                    if(settings->playerSettings->playerSettings[i] == NULL){
                        sprintf(logMsg, "Player %hd is already NULL", i);
                        log_stderr(0, 2, logMsg);
                    }

                    // free player
                    sprintf(logMsg, "Freeing player %hd", i);
                    log_stderr(0, 0, logMsg);
                    free(settings->playerSettings->playerSettings[i]);
                }
                // free player settings array;

            }

            // free player settings
            log_stderr(0, 0, "Freeing player array");
            free(settings->playerSettings->playerSettings);

        }else{
            log_stderr(0, 2, "Player settings is already NULL");
        }

        // free player settings
        log_stderr(0, 0, "Freeing player settings");
        free(settings->playerSettings);

    }else{
        log_stderr(0, 2, "Settings is already NULL");
    }

    // free settings

    log_stderr(0, 0, "Freeing settings");
    free(settings); // even tho settings could be null it could still be allocated
}

bool read_settings(char* fileName, struct settings_t* settings){

    log_stderr(0, 1, "Reading settings");
    char logMsg[80];
    FILE* settingsFile;


    if((settingsFile = fopen(fileName, "rb"))){
        sprintf(logMsg, "Successfully opened %s", fileName);
        log_stderr(0, 0, logMsg);
        // read game settings
        log_stderr(0, 1, "Reading game settings");
        fread(&settings->gameSettings, sizeof(struct gameSettings_t), 1, settingsFile);

        if(ferror(settingsFile)){
            // read error occured
            log_stderr(0, 3, "Read error occured");
            return false;
        }else if(feof(settingsFile)){
            // eof too early error
            log_stderr(0, 3, "Reached EOF too early");
            return false;
        }


        // read player settings
        log_stderr(0, 1, "Reading player settings");

        // allocate memory for player settings and start reading them
        settings->playerSettings = (struct playerSettings_t*)malloc(sizeof(struct playerSettings_t));

        if(settings->playerSettings == NULL){
            log_stderr(0, 3, "Memory allocation failed on player settings");
        }else{
            log_stderr(0, 0, "Successfully allocated memory for player settings");
        }

        // read arr size
        log_stderr(0, 0, "Reading player array size");
        fread(&settings->playerSettings->playerArrSize, sizeof(uint8_t), 1, settingsFile);

        if(ferror(settingsFile)){
            // read error occured
            log_stderr(0, 3, "Read error occured");
            return false;
        }else if(feof(settingsFile)){
            // eof too early error
            log_stderr(0, 3, "Reached EOF too early");
            return false;
        }else{
            sprintf(logMsg, "Read player array size -> %hd", settings->playerSettings->playerArrSize);
            log_stderr(0, 0, logMsg);
        }

        // allocate memory for playerSettings->playerSettings
        settings->playerSettings->playerSettings = (struct player_t**)malloc(settings->playerSettings->playerArrSize * sizeof(struct player_t*));
        if(settings->playerSettings->playerSettings == NULL){
            log_stderr(0, 3, "Memory allocation failed for player pointer array");
        }else{
            log_stderr(0, 0, "Successfully allocated memory for player pointer array");
        }

        // allocate player_t  for each player and read vars
        for(short i = 0; i<settings->playerSettings->playerArrSize; i++){

            settings->playerSettings->playerSettings[i] = (struct player_t*)malloc(sizeof(struct player_t));

            if(settings->playerSettings->playerSettings[i] == NULL){
                sprintf(logMsg, "settings->playerSettings->playerArrSize %hd", i+1);
                log_stderr(0, 3, logMsg);
            }else{
                sprintf(logMsg, "Successfully allocated memory for player %hd", i+1);
                log_stderr(0, 0, logMsg);
            }

            log_stderr(0, 0, "Reading player number");

            fread(&settings->playerSettings->playerSettings[i]->number, sizeof(uint8_t), 1, settingsFile);

            if(ferror(settingsFile)){
                // read error occured
                log_stderr(0, 3, "Read error occured");
                return false;
            }else if(feof(settingsFile)){
                // eof too early error
                log_stderr(0, 3, "Reached EOF too early");
                return false;
            }else{
                sprintf(logMsg, "Read player %hd number - %hd", i+1, settings->playerSettings->playerSettings[i]->number);
                log_stderr(0, 0, logMsg);
            }

            log_stderr(0, 0, "Reading player colour");

            fread(&settings->playerSettings->playerSettings[i]->colour, sizeof(char), 1, settingsFile);

            if(ferror(settingsFile)){
                // read error occured
                log_stderr(0, 3, "Read error occured");
                return false;
            }else if(feof(settingsFile)){
                // eof too early error
                log_stderr(0, 3, "Reached EOF too early");
                return false;
            }else{
                sprintf(logMsg, "Read player %hd colour - %c", i+1, settings->playerSettings->playerSettings[i]->colour);
                log_stderr(0, 0, logMsg);
            }

            log_stderr(0, 0, "Reading player symbol");

            fread(&settings->playerSettings->playerSettings[i]->symbol, sizeof(char), 1, settingsFile);

            if(ferror(settingsFile)){
                // read error occured
                log_stderr(0, 3, "Read error occured");
                return false;
            }else if(feof(settingsFile)){
                // eof too early error
                log_stderr(0, 3, "Reached EOF too early");
                return false;
            }else{
                sprintf(logMsg, "Read player %hd symbol - %c", i+1, settings->playerSettings->playerSettings[i]->symbol);
                log_stderr(0, 0, logMsg);
            }

            log_stderr(0, 0, "Reading player colour code length");

            fread(&settings->playerSettings->playerSettings[i]->colourCodeLen, sizeof(uint8_t), 1, settingsFile);

            if(ferror(settingsFile)){
                // read error occured
                log_stderr(0, 3, "Read error occured");
                return false;
            }else if(feof(settingsFile)){
                // eof too early error
                log_stderr(0, 3, "Reached EOF too early");
                return false;
            }else{
                sprintf(logMsg, "Read player %hd colour code length - %d", i+1, settings->playerSettings->playerSettings[i]->colourCodeLen);
                log_stderr(0, 0, logMsg);
            }

            log_stderr(0, 0, "Reading player colour code");

            fread(settings->playerSettings->playerSettings[i]->colourCode, sizeof(char), settings->playerSettings->playerSettings[i]->colourCodeLen, settingsFile);

            if(ferror(settingsFile)){
                // read error occured
                log_stderr(0, 3, "Read error occured");
                return false;
            }else if(feof(settingsFile)){
                // eof too early error
                log_stderr(0, 3, "Reached EOF too early");
                return false;
            }else{
                sprintf(logMsg, "Read player %hd symbol - %s", i+1, settings->playerSettings->playerSettings[i]->colourCode);
                log_stderr(0, 0, logMsg);
            }
        }

        // read file settings
        log_stderr(0, 1, "Reading file settings");

        // log
        log_stderr(0, 0, "Reading file settings -> log filename len");

        fread(&settings->fileSettings.logFileNameLen, sizeof(uint8_t), 1, settingsFile);

        if(ferror(settingsFile)){
            // read error occured
            log_stderr(0, 3, "Read error occured");
            return false;
        }else if(feof(settingsFile)){
            // eof too early error
            log_stderr(0, 3, "Reached EOF too early");
            return false;
        }else{
            sprintf(logMsg, "Read log filename length - %d", settings->fileSettings.logFileNameLen);
            log_stderr(0, 0, logMsg);
        }

        log_stderr(0, 0, "Reading file settings -> log filename");

        fread(settings->fileSettings.logFileName, sizeof(char), settings->fileSettings.logFileNameLen + 1, settingsFile);

        if(ferror(settingsFile)){
            // read error occured
            log_stderr(0, 3, "Read error occured");
            return false;
        }else if(feof(settingsFile)){
            // eof too early error
            log_stderr(0, 3, "Reached EOF too early");
            return false;
        }else{
            sprintf(logMsg, "Read log filename - %s", settings->fileSettings.logFileName);
            log_stderr(0, 0, logMsg);
        }

        // settings
        log_stderr(0, 0, "Reading file settings -> settings filename len");

        fread(&settings->fileSettings.settingsFileNameLen, sizeof(uint8_t), 1, settingsFile);

        if(ferror(settingsFile)){
            // read error occured
            log_stderr(0, 3, "Read error occured");
            return false;
        }else if(feof(settingsFile)){
            // eof too early error
            log_stderr(0, 3, "Reached EOF too early");
            return false;
        }else{
            sprintf(logMsg, "Read settings filename length - %d", settings->fileSettings.settingsFileNameLen);
            log_stderr(0, 0, logMsg);
        }

        log_stderr(0, 0, "Reading file settings -> settings filename");

        fread(settings->fileSettings.settingsFileName, sizeof(char), settings->fileSettings.settingsFileNameLen + 1, settingsFile);

        if(ferror(settingsFile)){
            // read error occured
            log_stderr(0, 3, "Read error occured");
            return false;
        }else if(feof(settingsFile)){
            // eof too early error
            log_stderr(0, 3, "Reached EOF too early");
            return false;
        }else{
            sprintf(logMsg, "Read settings filename - %s", settings->fileSettings.settingsFileName);
            log_stderr(0, 0, logMsg);
        }

        //stats
        log_stderr(0, 0, "Reading file settings -> stats filename len");

        if(ferror(settingsFile)){
            // read error occured
            log_stderr(0, 3, "Read error occured");
            return false;
        }else if(feof(settingsFile)){
            // eof too early error
            log_stderr(0, 3, "Reached EOF too early");
            return false;
        }else{
            sprintf(logMsg, "Read stats filename length - %d", settings->fileSettings.statsFileNameLen);
            log_stderr(0, 0, logMsg);
        }

        fread(&settings->fileSettings.statsFileNameLen, sizeof(uint8_t), 1, settingsFile);

        log_stderr(0, 0, "Reading file settings -> stats filename");

        fread(settings->fileSettings.statsFileName, sizeof(char), settings->fileSettings.statsFileNameLen + 1, settingsFile);

        if(ferror(settingsFile)){
            // read error occured
            log_stderr(0, 3, "Read error occured");
            return false;
        }else if(feof(settingsFile)){
            // eof too early error
            log_stderr(0, 3, "Reached EOF too early");
            return false;
        }else{
            sprintf(logMsg, "Read stats filename - %s", settings->fileSettings.statsFileName);
            log_stderr(0, 0, logMsg);
        }

    }else{
        sprintf(logMsg, "Failed opening %s", fileName);
        log_stderr(0, 3, logMsg);
        printf("Failed to create or open settings file!\n");
        return false;
    }

    // fclose + check if fails
    if(fclose(settingsFile) == 0){
        sprintf(logMsg, "Successfully closed %s", fileName);
        log_stderr(0, 0, logMsg);
    }else{
        sprintf(logMsg, "Error closing %s", fileName);
        log_stderr(0, 3, logMsg);
    }

    return true;
}

bool write_settings(char* fileName, struct settings_t* settings){

    log_stderr(0, 1, "Writing settings");
    char logMsg[80];

    FILE* settingsFile;

    if((settingsFile = fopen(fileName, "wb+"))){

        log_stderr(0, 0, "Writing game settings");
        fwrite(settings, sizeof(struct gameSettings_t), 1, settingsFile);

        // write player settings -> have to dereference pointers
        log_stderr(0, 0, "Writing player settings");
        log_stderr(0, 0, "Writing player arr size");
        fwrite(&settings->playerSettings->playerArrSize, sizeof(uint8_t), 1, settingsFile);
        for(short i = 0; i<settings->playerSettings->playerArrSize; i++){
            log_stderr(0, 0, "Writing player number");
            fwrite(&settings->playerSettings->playerSettings[i]->number, sizeof(uint8_t), 1, settingsFile);
            log_stderr(0, 0, "Writing player colour");
            fwrite(&settings->playerSettings->playerSettings[i]->colour, sizeof(char), 1, settingsFile);
            log_stderr(0, 0, "Writing player symbol");
            fwrite(&settings->playerSettings->playerSettings[i]->symbol, sizeof(char), 1, settingsFile);
            log_stderr(0, 0, "Writing player colour code length");
            fwrite(&settings->playerSettings->playerSettings[i]->colourCodeLen, sizeof(uint8_t), 1, settingsFile);
            log_stderr(0, 0, "Writing player colour code");
            fwrite(settings->playerSettings->playerSettings[i]->colourCode, sizeof(char), settings->playerSettings->playerSettings[i]->colourCodeLen, settingsFile);
        }

        // write file settings
        log_stderr(0, 0, "Writing file settings");

        // log
        log_stderr(0, 0, "Writing log file settings");
        fwrite(&settings->fileSettings.logFileNameLen, sizeof(uint8_t), 1, settingsFile);
        fwrite(settings->fileSettings.logFileName, sizeof(char), settings->fileSettings.logFileNameLen + 1, settingsFile);

        // settings
        log_stderr(0, 0, "Writing settings file settings");
        fwrite(&settings->fileSettings.settingsFileNameLen, sizeof(uint8_t), 1, settingsFile);
        fwrite(settings->fileSettings.settingsFileName, sizeof(char), settings->fileSettings.settingsFileNameLen + 1, settingsFile);

        //stats
        log_stderr(0, 0, "Writing stats file settings");
        fwrite(&settings->fileSettings.statsFileNameLen, sizeof(uint8_t), 1, settingsFile);
        fwrite(settings->fileSettings.statsFileName, sizeof(char), settings->fileSettings.statsFileNameLen + 1, settingsFile);

    }else{
        printf("Failed to create or open settings file!\n");
        log_stderr(0, 3, "Failed to create or open settings file");
        return false;
    }

    if(fclose(settingsFile) == 0){
        sprintf(logMsg, "Successfully closed %s", fileName);
        log_stderr(0, 0, logMsg);
    }else{
        sprintf(logMsg, "Failed to close %s", fileName);
        log_stderr(0, 3, logMsg);
        return false;
    }

    return true;
}

struct settings_t*  init_settings(char* fileName, struct dict_t* colourDict, bool* success){

    // add correct path to filename
    char editedFileName[strlen(fileName) + strlen("../res/bin/")];
    sprintf(editedFileName, "../res/bin/%s", fileName);
    struct settings_t* settings;

    // if file exists
    if(access(editedFileName, F_OK) == 0){ // test file existance -> access - if it can be accessed; F_OK if it exists

        log_stderr(0, 1, "Settings file already exists");

        settings = (struct settings_t*)malloc(sizeof(struct settings_t));
        // check if malloc was successful
        if(settings == NULL){
            log_stderr(0, 3, "Memory allocation failed on settings initialization");
            *success = false;
            return settings;
        }else{
            log_stderr(0, 0, "Successfully allocated memory for settings");
        }

        // read file to settings
        if(!read_settings(editedFileName, settings)){;
            printf("Failed to create or open settings file!\n");
            return NULL;
        }
    }else{
        log_stderr(0, 1, "Settings file doesn't exists");
        //file doesn't exist
        settings = define_settings(colourDict);
        if(settings == NULL){
            *success = false;
            return settings;
        }

        if(!write_settings(editedFileName, settings)){
            printf("Failed to create or open settings file!\n");
            *success = false;
            return settings;
        }
    }
    *success = true;
    return settings;
}

struct settings_t* define_settings(struct dict_t* colourDict){

    log_stderr(0, 1, "Defining settings");

    struct settings_t* settings = (struct settings_t*)malloc(sizeof(struct settings_t));
    // check if malloc fialed
    if(settings == NULL){
        log_stderr(0, 3, "Memory allocation failed for settings");
    }else{
        log_stderr(0, 0, "Successfully allocated memory for settings");
    }

    log_stderr(0, 0, "Creating game settings (with const values)");
    struct gameSettings_t gameSettings = {.maxRows = 20, .maxColumns = 40, .maxPlayers=9};

    // default values for game settings
    log_stderr(0, 0, "Assigning default values for game settings");

    gameSettings.boardRows = 6;
    gameSettings.boardColumns = 7;
    gameSettings.connectAmount = 4;
    gameSettings.againstBot = false;
    gameSettings.botDepth = 4; // O(40^4) worst case
    // gameSettings.multiplayer = false;
    gameSettings.playerAmount = 2;
    gameSettings.winHighlightColour = 'G';
    gameSettings.debugMode = 0;


    struct playerSettings_t* playerSettings = (struct playerSettings_t*)malloc(sizeof(struct playerSettings_t));
    // check if malloc fails
    if(playerSettings == NULL){
        log_stderr(0, 3, "Memory allocation failed for player settings");
        return NULL;
    }else{
        log_stderr(0, 0, "Successfully allocated memory for player settings");
    }

    // default values for player settings
    log_stderr(0, 0, "Assigning default values for player settings");

    playerSettings->playerArrSize = 2;

    playerSettings->playerSettings = (struct player_t**)malloc(playerSettings->playerArrSize * sizeof(struct player_t*));
    // check if malloc failed
    if(playerSettings->playerSettings == NULL){
        log_stderr(0, 3, "Memory allocation failed for player settings -> player settings pointer arr");
        return NULL;
    }else{
        log_stderr(0, 0, "Successfully allocated memory for player settings -> player settings pointer arr");
    }

    // player 1
    playerSettings->playerSettings[0] = (struct player_t*)malloc(sizeof(struct player_t));
    // check if malloc failed
    if(playerSettings->playerSettings[0] == NULL){
        log_stderr(0, 3, "Memory allocation failed for player settings -> player[0]");
        return NULL;
    }else{
        log_stderr(0, 0, "Successfully allocated memory for player settings -> player[0]");
    }

    log_stderr(0, 0, "Assigning default values for player[0]");
    playerSettings->playerSettings[0]->number = 1;
    playerSettings->playerSettings[0]->colour = 'R';
    playerSettings->playerSettings[0]->symbol = 'X';
    strcpy(playerSettings->playerSettings[0]->colourCode, binary_search_dict(playerSettings->playerSettings[0]->colour, colourDict));
    playerSettings->playerSettings[0]->colourCodeLen = strlen(playerSettings->playerSettings[0]->colourCode);

    // player 2
    playerSettings->playerSettings[1] = (struct player_t*)malloc(sizeof(struct player_t));
    // check if malloc failed
    if(playerSettings->playerSettings[0] == NULL){
        log_stderr(0, 3, "Memory allocation failed for player settings -> player[1]");
        return NULL;
    }else{
        log_stderr(0, 0, "Successfully allocated memory for player settings -> player[1]");
    }

    log_stderr(0, 0, "Assigning default values for player[1]");
    playerSettings->playerSettings[1]->number = 2;
    playerSettings->playerSettings[1]->colour = 'S';
    playerSettings->playerSettings[1]->symbol = 'O';
    strcpy(playerSettings->playerSettings[1]->colourCode, binary_search_dict(playerSettings->playerSettings[1]->colour, colourDict));
    playerSettings->playerSettings[1]->colourCodeLen = strlen(playerSettings->playerSettings[1]->colourCode);

    // default values for file settings

    // log
    log_stderr(0, 0, "Creating fileSettings");
    struct fileSettings_t fileSettings;

    log_stderr(0, 0, "Assigning default values for file settings");

    strcpy(fileSettings.logFileName, "log.txt\0");
    fileSettings.logFileNameLen = strlen(fileSettings.logFileName);

    // settings
    strcpy(fileSettings.settingsFileName, "settings.bin\0");
    fileSettings.settingsFileNameLen = strlen(fileSettings.settingsFileName);

    strcpy(fileSettings.statsFileName, "stats.bin\0");
    fileSettings.statsFileNameLen = strlen(fileSettings.statsFileName);

    // memcpy because gameSettings has const vars and cant assign it
    memcpy(&settings->gameSettings, &gameSettings, sizeof(struct gameSettings_t)); //memcpy doesnt fail so no need for checks
    settings->playerSettings = playerSettings;
    settings->fileSettings = fileSettings;

    printf("\n\n\n");
    return settings;
}

void display_settings_menu(struct settings_t* settings, struct dict_t* colourDict){

    log_stderr(0, 1, "Displaying settings menu");

    short settingsNO;
    bool exitCheck = false;

    log_stderr(0, 0, "Getting settings file path");

    char settingsFileName[settings->fileSettings.settingsFileNameLen + strlen("../res/bin/")];
    sprintf(settingsFileName, "../res/bin/%s", settings->fileSettings.settingsFileName);

    // colour vars
    log_stderr(0, 0, "Fetching colours from colour dict");
    const char *flash, *white, *def, *yellow, *heavy;
    flash = binary_search_dict('F', colourDict);
    white = binary_search_dict('W', colourDict);
    def = binary_search_dict('D', colourDict);
    heavy = binary_search_dict('H', colourDict);
    yellow = binary_search_dict('Y', colourDict);

    // stdin err msg
    char errmsg[] = {" \n Invalid input! Try again\n go to"};

    bool changedSettings = false;

    while(1){
        log_stderr(0, 0, "Printing main menu");
        system("clear");
        printf("%s%s CONNECT FOUR %s", heavy, yellow, def);
        printf("%s (settings) %s\n\n", yellow, def);
        printf(" %s-%s%s 1) GAME SETTINGS%s\n", flash, def, white, def);
        printf(" %s-%s%s 2) PLAYER SETTINGS%s\n", flash, def, white, def);
        printf(" %s-%s%s 3) FILE SETTINGS%s\n", flash, def, white, def);
        printf(" %s-%s%s 4) BACK%s\n", flash, def, white, def);



        // printf("log filename - %s\n", settings->fileSettings.logFileName);
        // printf("settings filename - %s\n", settings->fileSettings.settingsFileName);
        // printf("stats filename - %s\n", settings->fileSettings.statsFileName);


        printf(" \n go to: ");
        // get input and check if esc key was pressed
        exitCheck = !get_short(&settingsNO, 1, 4, errmsg);

        // exit condition
        if(exitCheck == true || settingsNO == 4){
            log_stderr(0, 0, "Exiting settings menu");
            if(changedSettings){

                // if settings were made write them
                write_settings(settingsFileName, settings);
            }

            break;
        }

        // Game settings menu
        if(settingsNO == 1){
            // display game settings menu
            if(display_game_settings_menu(settings, colourDict)){
                log_stderr(0, 0, "Acknowledging changes made in game settings menu");
                // if changes were made aknowledge it
                changedSettings = true;
            };
        }else if(settingsNO == 2){
            if(display_player_settings_menu(settings, colourDict)){
                log_stderr(0, 0, "Acknowledging changes made in player settings menu");
                changedSettings = true;
            }
        }else{
            if(display_file_settings_menu(settings, colourDict)){
                log_stderr(0, 0, "Acknowledging changes made in file settings menu");
                changedSettings = true;
            }
        }

    }

    // return;
}

bool display_game_settings_menu(struct settings_t* settings, struct dict_t* colourDict){

    log_stderr(0, 1, "Displaying game settings");

    short settingsNO;
    bool exitCheck = false;

    // colour vars
    log_stderr(0, 0, "Getting colours from colour dict");

    const char *flash, *white, *def, *yellow, *heavy;
    flash = binary_search_dict('F', colourDict);
    white = binary_search_dict('W', colourDict);
    def = binary_search_dict('D', colourDict);
    heavy = binary_search_dict('H', colourDict);
    yellow = binary_search_dict('Y', colourDict);
    bool settingsChange = false;
    char logMsg[50];

    // stdin err msges
    char errmsg1[] = {" \n Invalid input! Try again\n go to"};
    char errmsg2[] = {" \n Invalid input! Try again\n"};
    sprintf(errmsg2, "%s%s NEW VALUE: %s", heavy, white, def);

    // arr to hold all game settings info
    log_stderr(0, 0, "Creating settings array (with detailed information)");
    struct displaySettings_t settingsArr[] =
    {
        {
            .name = "board_rows",
            .description = "Sets the rows (height) of the board.",
            .value = &settings->gameSettings.boardRows,
            .minValue = 4,
            .maxValue = settings->gameSettings.maxRows,
        },
        {
            .name = "board_columns",
            .description = "Sets the columns (width) of the board.",
            .value = &settings->gameSettings.boardColumns,
            .minValue = 4,
            .maxValue = settings->gameSettings.maxColumns
        },
        {
            .name = "connect_amount",
            .description = "Sets how many nodes need to be connected for a win.",
            .value = &settings->gameSettings.connectAmount,
            .minValue = 3,
            .maxValue = 10
        },
        {
            .name = "win_highlight_colour",
            .description = "Sets colour that highlights winning nodes.",
            .value = &settings->gameSettings.winHighlightColour,
            .minValue = 1,
            .maxValue = colourDict->currentSize
        },
        {
            .name = "against_bot",
            .description = "[WIP]Sets your opponents to bots.",
            .value = &settings->gameSettings.againstBot,
            .minValue = 0,
            .maxValue = 1
        },
        {
            .name = "bot_depth",
            .description = "[WIP]Changes how many moves the bot calculates. Scales exponentially",
            .value = &settings->gameSettings.botDepth,
            .minValue = 1,
            .maxValue = 10
        },
        // {
        //     .name = "multiplayer",
        //     .description = "[WIP]Allows playing online via socket connections.",
        //     .value = &settings->gameSettings.multiplayer,
        //     .minValue = 0,
        //     .maxValue = 1
        // },
        {
            .name = "player_amount",
            .description = "Sets the amount of players in a game.",
            .value = &settings->gameSettings.playerAmount,
            .minValue = 2,
            .maxValue = 9
        },
        {
            .name = "debug_mode",
            .description = "Sets the logging detail. Takes instant effect (still have to go to main menu to save settings)",
            .value = &settings->gameSettings.debugMode,
            .minValue = 0,
            .maxValue = 1
        },

        {
            .name = "BACK" // also used for determining when settings end
        }
    };

    short i;

    while(1){

        system("clear");

        printf("%s%s CONNECT FOUR %s", heavy, yellow, def);
        printf("%s (game-settings) %s\n\n", yellow, def);

        // print all setting name-value from settingsArr
        log_stderr(0, 0, "Printing game settings");

        for(i = 0; strcmp(settingsArr[i].name, "BACK") != 0; i++){
            // show win highligh colour
            if(strcmp(settingsArr[i].name, "win_highlight_colour") == 0){
                printf(" %s-%s%s %hd) %s - %s'%c'%s \n", flash, def, white, i+1, settingsArr[i].name, binary_search_dict(*(char*)settingsArr[i].value, colourDict), *(char*)settingsArr[i].value, def);
            }else{
            // show every other setting
                printf(" %s-%s%s %hd) %s - %hd%s\n", flash, def, white, i+1, settingsArr[i].name, *(short*)settingsArr[i].value, def);
            }

        }
        // print BACK
        printf(" %s-%s%s %d) %s%s\n", flash, def, white, i+1, settingsArr[i].name, def);

        printf("\n go to: ");
        // get input and check if esc key was pressed
        exitCheck = !get_short(&settingsNO, 1, i+1, errmsg1);



        // exit condition
        if(settingsNO == i+1 || exitCheck == true){
            log_stderr(0, 1, "Exiting game settings");
            break;
        }

        // print additional info about settings
        sprintf(logMsg, "Displaying game settings - %s menu", settingsArr[settingsNO-1].name);
        log_stderr(0, 1, logMsg);
        sprintf(logMsg, "Printing additional info about %s", settingsArr[settingsNO-1].name);
        log_stderr(0, 0, logMsg);

        system("clear");

        printf("%s%s CONNECT FOUR %s", heavy, yellow, def);
        printf("%s (game-settings-%s) %s\n\n", yellow, settingsArr[settingsNO-1].name, def);

        printf("%s%s NAME: %s %s\n", heavy, white, def, settingsArr[settingsNO - 1].name);
        printf("%s%s DESCRIPTION: %s %s\n", heavy, white, def, settingsArr[settingsNO - 1].description);
        printf("%s%s VALUE RANGE: %s (%hd-%hd)\n", heavy, white, def, settingsArr[settingsNO - 1].minValue, settingsArr[settingsNO - 1].maxValue);

        // different print msg for win highlight
        if(strcmp(settingsArr[settingsNO - 1].name, "win_highlight_colour") == 0){
            print_colour_dict(colourDict);
            printf("\n\n%s%s CURRENT VALUE:%s '%c'\n", heavy, white, def, *(short*)settingsArr[settingsNO - 1].value);
            printf("\n%s%s NEW VALUE: %s", heavy, white, def);
        // different print msg for change debug mode
    }else if(strcmp(settingsArr[settingsNO - 1].name, "debug_mode") == 0){
            printf("%s%s CURRENT VALUE: %s %hd\n", heavy, white, def, log_stderr(0, 0, "Fetching debug mode"));
            printf("\n%s%s CHANGE VALUE %s(1-yes / 0-no / ESC-no): ", heavy, white, def);
        }else{
            printf("%s%s CURRENT VALUE: %s %hd\n", heavy, white, def, *(short*)settingsArr[settingsNO - 1].value);
            printf("\n%s%s NEW VALUE: %s", heavy, white, def);
        }


        // if new value is assigned change settingsChange to true

        if(settingsArr[settingsNO - 1].maxValue == 1 && settingsArr[settingsNO - 1].minValue == 0){

            log_stderr(0, 0, "Assigning bool value");
            if(get_bool(settingsArr[settingsNO-1].value, errmsg2)){
                sprintf(logMsg, "Value changed to %hd", *(bool*)settingsArr[settingsNO-1].value);
                log_stderr(0, 1, logMsg);
                settingsChange = true;

                // change debug mode instantly (also change game settings debug mode to new debug mode)
                if(strcmp(settingsArr[settingsNO - 1].name, "debug_mode") == 0){
                    settings->gameSettings.debugMode = log_stderr(*(bool*)settingsArr[settingsNO - 1].value, 0, "Getting debug and (maybe) changing debug mode based on user input");
                }
            }
        }else{
            log_stderr(0, 0, "Assigning short value");

            if(get_short(settingsArr[settingsNO-1].value, settingsArr[settingsNO-1].minValue, settingsArr[settingsNO-1].maxValue, errmsg2)){
                sprintf(logMsg, "Value changed to %hd", *(short*)settingsArr[settingsNO-1].value);
                log_stderr(0, 1, logMsg);
                settingsChange = true;

                // special case for char val setting -> change value to value from colour dict
                if(strcmp(settingsArr[settingsNO-1].name, "win_highlight_colour") == 0){
                    *(char*)settingsArr[settingsNO-1].value = colourDict->nodeArr[*(short*)settingsArr[settingsNO-1].value-1].key;

                // case for changing player amount
                }else if(strcmp(settingsArr[settingsNO-1].name, "player_amount") == 0){

                    // check if player amount > playerArrSize
                    if(*(short*)settingsArr[settingsNO-1].value > settings->playerSettings->playerArrSize){

                        log_stderr(0, 2, "Can't change player amount - not enough players defined");

                        printf("player array size - %hd", settings->playerSettings->playerArrSize);
                        printf("\n ERROR - Not enough players defined. Add more via the player settings menu. (value set to 2)");
                        *(short*)settingsArr[settingsNO-1].value = 2;
                        getchar(); // wait for user input
                    }
                }
            }

        }
    }
    return settingsChange;
}


bool display_player_settings_menu(struct settings_t* settings, struct dict_t* colourDict){

    log_stderr(0, 1, "Displaying player settings");

    short settingsNO, settingsNO2, settingsNO3;
    bool exitCheck, exitCheck2, exitCheck3, settingsChange;
    exitCheck = exitCheck2 = exitCheck3 = settingsChange = false;

    char logMsg[50];
    log_stderr(0, 0, "Fetching colours from colour dict");
    const char *flash, *white, *def, *yellow, *heavy;
    flash = binary_search_dict('F', colourDict);
    white = binary_search_dict('W', colourDict);
    def = binary_search_dict('D', colourDict);
    heavy = binary_search_dict('H', colourDict);
    yellow = binary_search_dict('Y', colourDict);

    char errmsg1[] = {" \n Invalid input! Try again\n go to"};
    char errmsg2[] = {" \n Invalid input! Try again\n"};


    short i;

    while(1){

        system("clear");

        printf("%s%s CONNECT FOUR %s", heavy, yellow, def);
        printf("%s (player-settings) %s\n\n", yellow, def);

        // print all players
        log_stderr(0, 0, "Printing all players");
        for(i = 0; i<settings->playerSettings->playerArrSize; i++){
            printf(" %s-%s%s %hd) PLAYER %hd %s\n", flash, def, white, i+1, settings->playerSettings->playerSettings[i]->number, def);
            printf("        symbol: %s%c%s\n", settings->playerSettings->playerSettings[i]->colourCode, settings->playerSettings->playerSettings[i]->symbol, def);
        }

        // back and add player
        printf(" %s-%s%s %hd) ADD PLAYER%s\n", flash, def, white, i+1, def);
        printf(" %s-%s%s %hd) BACK%s\n", flash, def, white, i+2, def);

        printf("\n go to: ");
        exitCheck = !get_short(&settingsNO, 1, i+2, errmsg1);

        // exit condition
        if(settingsNO == i+2 || exitCheck == true){
            log_stderr(0, 1, "Exiting player settings");
            break;
        }else if(settingsNO == i+1){ // add player
            sprintf(logMsg, "Adding player %d", settings->playerSettings->playerArrSize+1);
            log_stderr(0, 0, logMsg);

            if(settings->playerSettings->playerArrSize >= 9){
                log_stderr(0, 2, "Max amount of players already reached");
                printf("ERROR - already reached max player amount");
                getchar();
            }else{
                add_player(settings->playerSettings, colourDict);
                settingsChange = true;
            }

        }else{
            // choose which player setting u want to change (colour/symbol)
            sprintf(logMsg, "Displaying menu for player %d", settingsNO);
            log_stderr(0, 1, logMsg);
            while(1){

                log_stderr(0, 0, "Printing menu info");
                system("clear");

                printf("%s%s CONNECT FOUR %s", heavy, yellow, def);
                printf("%s (player_%hd-settings) %s\n\n", yellow, settingsNO, def);

                printf(" %s-%s%s 1) change colour %s\n", flash, def, white, def);
                printf(" %s-%s%s 2) change symbol %s\n", flash, def, white, def);
                printf(" %s-%s%s 3) BACK %s\n", flash, def, white, def);

                printf("\n go to: ");
                exitCheck2 = !get_short(&settingsNO2, 1, 3, errmsg1);

                // exit to player-settings menu
                if(settingsNO2 == 3 || exitCheck2 == true){
                    break;

                }else{
                    if(settingsNO2 == 1){
                        // change colour
                        log_stderr(0, 1, "Displaying player colour menu");
                        log_stderr(0, 0, "Printing player colour info");
                        system("clear");

                        printf("%s%s CONNECT FOUR %s", heavy, yellow, def);
                        printf("%s (player_%hd-settings-symbol_colour) %s\n\n", yellow, settingsNO, def);

                        print_colour_dict(colourDict);

                        printf("\n\n choose colourNO: ");
                        exitCheck3 = !get_short(&settingsNO3, 1, colourDict->currentSize, errmsg2);

                        if(exitCheck3 == true){
                            // exit to player_n-settings
                            sprintf(logMsg, "Exiting to menu for player %d", settingsNO);
                            log_stderr(0, 1, logMsg);
                            continue;
                        }else{

                            log_stderr(0, 0, "Adding colour info to player");

                            settings->playerSettings->playerSettings[settingsNO - 1]->colour = colourDict->nodeArr[settingsNO3 - 1].key;
                            strcpy(settings->playerSettings->playerSettings[settingsNO - 1]->colourCode, colourDict->nodeArr[settingsNO3 - 1].value);
                            settings->playerSettings->playerSettings[settingsNO - 1]->colourCodeLen = strlen(settings->playerSettings->playerSettings[settingsNO2 - 1]->colourCode);

                            settingsChange = true;

                            // exit to player-settings
                            log_stderr(0, 1, "Exiting to player-settings");
                            break;
                        }

                    }else{ // == 2
                        log_stderr(0, 1, "Displaying player symbol menu");
                        log_stderr(0, 0, "Printing player symbol info");
                        system("clear");
                        printf("%s%s CONNECT FOUR %s", heavy, yellow, def);
                        printf("%s (player_%hd-settings-symbol_letter) %s\n\n", yellow, settingsNO, def);

                        printf("\n\n choose symbol: ");
                        settings->playerSettings->playerSettings[settingsNO-1]->symbol = getchar();

                        sprintf(logMsg, "Changed player %hd symbol to %c[%hd]", settingsNO, settings->playerSettings->playerSettings[settingsNO-1]->symbol, settings->playerSettings->playerSettings[settingsNO-1]->symbol);
                        log_stderr(0, 1, logMsg);
                        // gets only the first character of msg -> eat the rest if any
                        clear_stdin();


                        settingsChange = true;

                        // exit to player-settings
                        log_stderr(0, 1, "Exiting to player-settings");
                        break;
                    }
                }
            }
        }
    }

    return settingsChange;
}

bool display_file_settings_menu(struct settings_t* settings, struct dict_t* colourDict){

    log_stderr(0, 1, "Displaying file settings menu");

    bool exitCheck;
    bool settingsChange = false;
    short settingsNO;
    char* errmsg = "\n Invalid input! Try again\n go to";
    char logMsg[55];

    // fetch colours
    log_stderr(0, 0, "Fetching colours from colour dict");
    const char *flash, *white, *def, *yellow, *heavy;
    flash = binary_search_dict('F', colourDict);
    white = binary_search_dict('W', colourDict);
    def = binary_search_dict('D', colourDict);
    heavy = binary_search_dict('H', colourDict);
    yellow = binary_search_dict('Y', colourDict);

    while(1){
        log_stderr(0, 0, "Printing fuke settings menu");
        system("clear");

        printf("%s%s CONNECT FOUR %s", heavy, yellow, def);
        printf("%s (file-settings) %s\n\n", yellow, def);

        printf(" %s-%s%s 1) change log name (%s) %s\n", flash, def, white, settings->fileSettings.logFileName, def);
        printf(" %s-%s%s 2) BACK %s\n", flash, def, white, def);

        exitCheck = !get_short(&settingsNO, 1, colourDict->currentSize, errmsg);

        if(settingsNO == 2 || exitCheck == true){
            log_stderr(0, 1, "Exiting to main-menu");
            break;
        }

        if(settingsNO == 1){
            log_stderr(0, 1, "Displaying file settings-log menu");
            system("clear");

            printf("%s%s CONNECT FOUR %s", heavy, yellow, def);
            printf("%s (file-settings-log) %s\n\n", yellow, def);

            printf("\n%s%sNEW FILENAME: %s", heavy, white, def);

            if(get_validated_str(settings->fileSettings.logFileName, 1, 31-5, errmsg)){
                settings->fileSettings.logFileNameLen = strlen(settings->fileSettings.logFileName);
                settingsChange = true;
                sprintf(logMsg, "Changed value to %s", settings->fileSettings.logFileName);
                log_stderr(0, 1, logMsg);
                continue;
            }else{
                log_stderr(0, 0, "Value unchanged");
                continue; // loop again
            }
        }
    }


    return settingsChange;

}

/* -------------------------------------------------------------------------- */
