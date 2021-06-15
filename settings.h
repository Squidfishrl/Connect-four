/* IMPORTED LIBRARIES */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#include "colours.h"
#include "get_stdin.h"

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

    bool multiplayer; // f -> local; t -> TODO: socket connection

    short playerAmount; // rewrite to feature more players
    const short maxPlayers;

    char winHighlightColour;

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
    char colourCode[10]; // would be nice if it could be a pointer but I keep getting fread errors when it is
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

struct settings_t* init_settings(char* fileName, struct dict_t* colourDict);
struct settings_t* define_settings(struct dict_t* colourDict); // create default settings
bool read_settings(char* fileName, struct settings_t* settings);
bool change_setting(struct settings_t* settings, short settingNO); // TODO: useless - remove
bool write_settings(char* fileName, struct settings_t* settings);
void free_settings(struct settings_t* settings); // TODO:

void display_settings_menu(struct settings_t* settings, struct dict_t* colourDict); // NOTE: could add filename depending what I do in it
bool display_game_settings_menu(struct settings_t* settings, struct dict_t* colourDict); // returns if changes were made

/* -------------------------------------------------------------------------- */


/* FUNCTION DEFINITIONS */

void free_settings(struct settings_t* settings){
    // TODO:
}

bool read_settings(char* fileName, struct settings_t* settings){

    FILE* settingsFile;

    if((settingsFile = fopen(fileName, "rb"))){
        // read game settings
        fread(&settings->gameSettings, sizeof(struct gameSettings_t), 1, settingsFile);

        // read player settings

        // allocate memory for player settings and start reading them
        settings->playerSettings = (struct playerSettings_t*)malloc(sizeof(struct playerSettings_t));

        // read arr size
        fread(&settings->playerSettings->playerArrSize, sizeof(uint8_t), 1, settingsFile);

        // allocate memory for playerSettings->playerSettings
        settings->playerSettings->playerSettings = (struct player_t**)malloc(settings->playerSettings->playerArrSize * sizeof(struct player_t*));

        // allocate player_t  for each player and read vars
        for(short i = 0; i<settings->playerSettings->playerArrSize; i++){
            settings->playerSettings->playerSettings[i] = (struct player_t*)malloc(sizeof(struct player_t));
            fread(&settings->playerSettings->playerSettings[i]->number, sizeof(uint8_t), 1, settingsFile);
            fread(&settings->playerSettings->playerSettings[i]->colour, sizeof(char), 1, settingsFile);
            fread(&settings->playerSettings->playerSettings[i]->symbol, sizeof(char), 1, settingsFile);
            fread(&settings->playerSettings->playerSettings[i]->colourCodeLen, sizeof(uint8_t), 1, settingsFile);
            fread(settings->playerSettings->playerSettings[i]->colourCode, sizeof(char), settings->playerSettings->playerSettings[i]->colourCodeLen, settingsFile);
        }

        // read file settings

        // log
        fread(&settings->fileSettings.logFileNameLen, sizeof(uint8_t), 1, settingsFile);
        fread(settings->fileSettings.logFileName, sizeof(char), settings->fileSettings.logFileNameLen + 1, settingsFile);

        // settings
        fread(&settings->fileSettings.settingsFileNameLen, sizeof(uint8_t), 1, settingsFile);
        fread(settings->fileSettings.settingsFileName, sizeof(char), settings->fileSettings.settingsFileNameLen + 1, settingsFile);


        //stats
        fread(&settings->fileSettings.statsFileNameLen, sizeof(uint8_t), 1, settingsFile);
        fread(settings->fileSettings.statsFileName, sizeof(char), settings->fileSettings.statsFileNameLen + 1, settingsFile);

    }else{
        printf("Failed to create or open settings file!\n");
        return false;
    }

    fclose(settingsFile);

    return true;
}

bool write_settings(char* fileName, struct settings_t* settings){
    FILE* settingsFile;

    if((settingsFile = fopen(fileName, "wb+"))){

        fwrite(settings, sizeof(struct gameSettings_t), 1, settingsFile);

        // write player settings -> have to dereference pointers
        fwrite(&settings->playerSettings->playerArrSize, sizeof(uint8_t), 1, settingsFile);
        for(short i = 0; i<settings->playerSettings->playerArrSize; i++){
            fwrite(&settings->playerSettings->playerSettings[i]->number, sizeof(uint8_t), 1, settingsFile);
            fwrite(&settings->playerSettings->playerSettings[i]->colour, sizeof(char), 1, settingsFile);
            fwrite(&settings->playerSettings->playerSettings[i]->symbol, sizeof(char), 1, settingsFile);
            fwrite(&settings->playerSettings->playerSettings[i]->colourCodeLen, sizeof(uint8_t), 1, settingsFile);
            fwrite(settings->playerSettings->playerSettings[i]->colourCode, sizeof(char), settings->playerSettings->playerSettings[i]->colourCodeLen, settingsFile);
        }

        // write file settings

        // log
        fwrite(&settings->fileSettings.logFileNameLen, sizeof(uint8_t), 1, settingsFile);
        fwrite(settings->fileSettings.logFileName, sizeof(char), settings->fileSettings.logFileNameLen + 1, settingsFile);

        // settings
        fwrite(&settings->fileSettings.settingsFileNameLen, sizeof(uint8_t), 1, settingsFile);
        fwrite(settings->fileSettings.settingsFileName, sizeof(char), settings->fileSettings.settingsFileNameLen + 1, settingsFile);

        //stats
        fwrite(&settings->fileSettings.statsFileNameLen, sizeof(uint8_t), 1, settingsFile);
        fwrite(settings->fileSettings.statsFileName, sizeof(char), settings->fileSettings.statsFileNameLen + 1, settingsFile);

    }else{
        printf("Failed to create or open settings file!\n");
        return false;
    }


    fclose(settingsFile);
    return true;
}

struct settings_t* init_settings(char* fileName, struct dict_t* colourDict){

    struct settings_t* settings;

    // file exists
    if(access(fileName, F_OK) == 0){


        settings = (struct settings_t*)malloc(sizeof(struct settings_t));

        // read file to settings
        if(!read_settings(fileName, settings)){
            printf("Failed to create or open settings file!\n");
            return NULL;
        }
    }else{
        //file doesn't exist

        settings = define_settings(colourDict);
        if(!write_settings(fileName, settings)){
            printf("Failed to create or open settings file!\n");
        }
    }

    return settings;
}

struct settings_t* define_settings(struct dict_t* colourDict){

    struct gameSettings_t gameSettings = {.maxRows = 20, .maxColumns = 40, .maxPlayers=9};
    struct playerSettings_t* playerSettings = (struct playerSettings_t*)malloc(sizeof(struct playerSettings_t));
    struct fileSettings_t fileSettings;
    struct settings_t* settings = (struct settings_t*)malloc(sizeof(struct settings_t));



    // default values for game settings
    gameSettings.boardRows = 20;
    gameSettings.boardColumns = 16;
    gameSettings.connectAmount = 4;
    gameSettings.againstBot = false;
    gameSettings.botDepth = 4; // O(40^4) worst case
    gameSettings.multiplayer = false;
    gameSettings.playerAmount = 2;
    gameSettings.winHighlightColour = 'G';

    // default values for player settings
    playerSettings->playerArrSize = 2;
    playerSettings->playerSettings = (struct player_t**)malloc(playerSettings->playerArrSize * sizeof(struct player_t*));


    // player 1
    playerSettings->playerSettings[0] = (struct player_t*)malloc(sizeof(struct player_t));
    playerSettings->playerSettings[0]->number = 1;
    playerSettings->playerSettings[0]->colour = 'R';
    playerSettings->playerSettings[0]->symbol = 'X';
    strcpy(playerSettings->playerSettings[0]->colourCode, binary_search_dict(playerSettings->playerSettings[0]->colour, colourDict));
    // playerSettings->playerSettings[0]->colourCode = binary_search_dict(playerSettings->playerSettings[0]->colour, colourDict);
    playerSettings->playerSettings[0]->colourCodeLen = strlen(playerSettings->playerSettings[0]->colourCode);

    // player 2
    playerSettings->playerSettings[1] = (struct player_t*)malloc(sizeof(struct player_t));
    playerSettings->playerSettings[1]->number = 2;
    playerSettings->playerSettings[1]->colour = 'S';
    playerSettings->playerSettings[1]->symbol = 'O';
    strcpy(playerSettings->playerSettings[1]->colourCode, binary_search_dict(playerSettings->playerSettings[1]->colour, colourDict));
    // playerSettings->playerSettings[1]->colourCode = binary_search_dict(playerSettings->playerSettings[1]->colour, colourDict);
    playerSettings->playerSettings[1]->colourCodeLen = strlen(playerSettings->playerSettings[1]->colourCode);

    // default values for file settings

    // log
    strcpy(fileSettings.logFileName, "log.txt\0");
    fileSettings.logFileNameLen = strlen(fileSettings.logFileName);

    // settings
    strcpy(fileSettings.settingsFileName, "settings.bin\0");
    fileSettings.settingsFileNameLen = strlen(fileSettings.settingsFileName);

    strcpy(fileSettings.statsFileName, "stats.bin\0");
    fileSettings.statsFileNameLen = strlen(fileSettings.statsFileName);

    // memcpy because gameSettings has const vars and cant assign it
    memcpy(&settings->gameSettings, &gameSettings, sizeof(struct gameSettings_t));
    settings->playerSettings = playerSettings;
    settings->fileSettings = fileSettings;

    printf("\n\n\n");
    return settings;
}

void display_settings_menu(struct settings_t* settings, struct dict_t* colourDict){

    short settingsNO;
    const char *flash, *white, *def, *yellow, *heavy;
    flash = binary_search_dict('F', colourDict);
    white = binary_search_dict('W', colourDict);
    def = binary_search_dict('D', colourDict);
    heavy = binary_search_dict('H', colourDict);
    yellow = binary_search_dict('Y', colourDict);
    char errmsg[] = {" \n Invalid input! Try again\n go to"};
    bool changedSettings = false;

    while(1){

        system("clear");
        printf("%s%s CONNECT FOUR %s", heavy, yellow, def);
        printf("%s (settings) %s\n\n", yellow, def);
        printf(" %s-%s%s 1) GAME SETTINGS%s\n", flash, def, white, def);
        printf(" %s-%s%s 2) PLAYER SETTINGS%s\n", flash, def, white, def);
        printf(" %s-%s%s 3) FILE SETTINGS%s\n", flash, def, white, def);
        printf(" %s-%s%s 4) BACK%s\n", flash, def, white, def);


        // printf player and file stats
        // printf("player arr size - %hd\n\n", settings->playerSettings->playerArrSize);
        //
        // for(short i =0; i<settings->playerSettings->playerArrSize; i++){
        //     printf("player %hd\n", settings->playerSettings->playerSettings[i]->number);
        //     printf("player colour - %c\n", settings->playerSettings->playerSettings[i]->colour);
        //     printf("player symbol - %c\n\n", settings->playerSettings->playerSettings[i]->symbol);
        // }
        // printf("\n\n");


        // printf("log filename - %s\n", settings->fileSettings.logFileName);
        // printf("settings filename - %s\n", settings->fileSettings.settingsFileName);
        // printf("stats filename - %s\n", settings->fileSettings.statsFileName);

        // not in do while loop to allow error msg;


        printf(" \n go to: ");
        get_short(&settingsNO, 1, 4, errmsg);


        switch(settingsNO){
            case 1:
                if(display_game_settings_menu(settings, colourDict)){
                    changedSettings = true;
                };
                break;
            case 2:

                break;
            case 3:

                break;
            case 4:
                if(changedSettings){
                    write_settings(settings->fileSettings.settingsFileName, settings);
                }
                return;
            default:
                // shouldn't be possible with scanf validation
                break;
        }
    }

    return;
}

bool display_game_settings_menu(struct settings_t* settings, struct dict_t* colourDict){

    short settingsNO = -1;
    const char *flash, *white, *def, *yellow, *heavy;
    flash = binary_search_dict('F', colourDict);
    white = binary_search_dict('W', colourDict);
    def = binary_search_dict('D', colourDict);
    heavy = binary_search_dict('H', colourDict);
    yellow = binary_search_dict('Y', colourDict);
    bool settingsChange = true; // TODO: change to false once scanf validation funct
    char errmsg1[] = {" \n Invalid input! Try again\n go to"};
    char errmsg2[] = {" \n Invalid input! Try again\n"};
    strcat(errmsg2, heavy);
    strcat(errmsg2, white);
    strcat(errmsg2, " NEW VALUE: ");
    strcat(errmsg2, def);

    struct displaySettings_t settingsArr[] =
    {
        {
            .name = "board rows",
            .description = "Sets the rows (height) of the board.",
            .value = &settings->gameSettings.boardRows,
            .minValue = 4,
            .maxValue = settings->gameSettings.maxRows,
        },
        {
            .name = "board columns",
            .description = "Sets the columns (width) of the board.",
            .value = &settings->gameSettings.boardColumns,
            .minValue = 4,
            .maxValue = settings->gameSettings.maxColumns
        },
        {
            .name = "connect amount",
            .description = "Sets how many nodes need to be connected for a win.",
            .value = &settings->gameSettings.connectAmount,
            .minValue = 3,
            .maxValue = 10
        },
        {
            .name = "against bot",
            .description = "[WIP]Sets your opponents to bots.",
            .value = &settings->gameSettings.againstBot,
            .minValue = 0,
            .maxValue = 1
        },
        {
            .name = "bot depth",
            .description = "[WIP]Changes how many moves the bot calculates. Scales exponentially",
            .value = &settings->gameSettings.botDepth,
            .minValue = 1,
            .maxValue = 10
        },
        {
            .name = "multiplayer",
            .description = "[WIP]Allows playing online via socket connections.",
            .value = &settings->gameSettings.multiplayer,
            .minValue = 0,
            .maxValue = 1
        },
        {
            .name = "player amount",
            .description = "[WIP]Sets the amount of players in a game.",
            .value = &settings->gameSettings.playerAmount,
            .minValue = 2,
            .maxValue = 10
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

        for(i = 0; strcmp(settingsArr[i].name, "BACK") != 0; i++){
            printf(" %s-%s%s %d) %s - %hd%s\n", flash, def, white, i+1, settingsArr[i].name, *(short*)settingsArr[i].value, def);
        }
        printf(" %s-%s%s %d) %s%s\n", flash, def, white, i+1, settingsArr[i].name, def);

        printf("\n go to: ");
        get_short(&settingsNO, 1, i+1, errmsg1);
        system("clear");

        if(settingsNO == i+1){
            break;
        }

        printf("%s%s CONNECT FOUR %s", heavy, yellow, def);
        printf("%s (game-settings-configurations) %s\n\n", yellow, def);

        printf("%s%s NAME: %s %s\n", heavy, white, def, settingsArr[settingsNO - 1].name);
        printf("%s%s DESCRIPTION: %s %s\n", heavy, white, def, settingsArr[settingsNO - 1].description);
        printf("%s%s VALUE RANGE: %s (%hd-%hd)\n", heavy, white, def, settingsArr[settingsNO - 1].minValue, settingsArr[settingsNO - 1].maxValue);
        printf("%s%s CURRENT VALUE: %s %hd\n", heavy, white, def, *(short*)settingsArr[settingsNO - 1].value);
        printf("\n%s%s NEW VALUE: %s", heavy, white, def);

        // if new value is assigned change settingsChange to true
        if(settingsArr[settingsNO - 1].maxValue == 1 && settingsArr[settingsNO - 1].minValue == 0){

            if(get_bool(settingsArr[settingsNO-1].value, errmsg2)){
                settingsChange = true;
            }
        }else{
            if(get_short(settingsArr[settingsNO-1].value, settingsArr[settingsNO-1].minValue, settingsArr[settingsNO-1].maxValue, errmsg2)){
                settingsChange = true;
            }
        }

    }


    return settingsChange;
}


/* -------------------------------------------------------------------------- */
