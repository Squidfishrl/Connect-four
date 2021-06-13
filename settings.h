/* IMPORTED LIBRARIES */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

/* -------------------------------------------------------------------------- */


/* STRUCTURE DEFINITIONS */

struct gameSettings_t{

    uint8_t boardRows;
    uint8_t boardColumns;

    const short maxRows; // 20
    const short maxColumns; // 40

    bool againstBot; // TODO: add bot option, simple algorythm O(columns^2depth)
    uint8_t botDepth;

    bool multiplayerMode; // f -> local; t -> TODO: socket connection

    uint8_t playerAmount; // rewrite to feature more players
    const uint8_t maxPlayers;

    char winHighlightColour;

    /* OPT: if find a way to change terminal font size add settings for that
    setting could be read by a bash script if not possible in c
    */

};

struct fileSettings_t{
    char logFileName[31]; // should be private and always log but rules disagree..
    uint8_t logFileNameLen;
    char settingsFileName[31]; // following that logic
    uint8_t settingsFileNameLen;
    char statsFileName[31];
    uint8_t statsFileNameLen;
};

struct player_t{

    // TODO: in board.h change int type to player_t* type.. make it compatible
    uint8_t number;
    char colour;
    char symbol;
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

/* -------------------------------------------------------------------------- */


/* FUNCTION DECLARATIONS */

struct settings_t* init_settings(char* fileName);
struct settings_t* define_settings(); // create default settings
bool read_settings(char* fileName, struct settings_t* settings);
bool change_setting(struct settings_t* settings, short settingNO); // NOTE: if editing player settings and adding a setting for a new player, check if the player is actually new or not
bool write_settings(char* fileName, struct settings_t* settings);
void display_settings_menu(struct settings_t* settings); // could add filename depending what I do in it
void free_settings(struct settings_t* settings);

/* -------------------------------------------------------------------------- */


/* FUNCTION DEFINITIONS */

bool read_settings(char* fileName, struct settings_t* settings){
    // TODO: cant read anything when it comes to the array - fix
    FILE* settingsFile;

    if((settingsFile = fopen(fileName, "rb"))){
        // read game settings
        fread(&settings->gameSettings, sizeof(struct gameSettings_t), 1, settingsFile);

        // read player settings
        settings->playerSettings = (struct playerSettings_t*)malloc(sizeof(struct playerSettings_t));
        fread(&settings->playerSettings->playerArrSize, sizeof(uint8_t), 1, settingsFile);
        settings->playerSettings->playerSettings = (struct player_t**)malloc(settings->playerSettings->playerArrSize * sizeof(struct player_t*));
        for(short i = 0; i<settings->playerSettings->playerArrSize; i++){
            settings->playerSettings->playerSettings[i] = (struct player_t*)malloc(sizeof(struct player_t));
            fread(&settings->playerSettings->playerSettings[i]->number, sizeof(uint8_t), 1, settingsFile);
            fread(&settings->playerSettings->playerSettings[i]->colour, sizeof(char), 1, settingsFile);
            fread(&settings->playerSettings->playerSettings[i]->symbol, sizeof(char), 1, settingsFile);
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

struct settings_t* init_settings(char* fileName){

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

        settings = define_settings();
        if(!write_settings(fileName, settings)){
            printf("Failed to create or open settings file!\n");
        }
    }

    return settings;
}

struct settings_t* define_settings(){

    struct gameSettings_t gameSettings = {.maxRows = 20, .maxColumns = 40, .maxPlayers=9};
    struct playerSettings_t* playerSettings = (struct playerSettings_t*)malloc(sizeof(struct playerSettings_t));
    struct fileSettings_t fileSettings;
    struct settings_t* settings = (struct settings_t*)malloc(sizeof(struct settings_t));



    // default values for game settings
    gameSettings.boardRows = 6;
    gameSettings.boardColumns = 7;
    gameSettings.againstBot = false;
    gameSettings.botDepth = 4; // O(40^4) worst case
    gameSettings.multiplayerMode = false;
    gameSettings.playerAmount = 2;
    gameSettings.winHighlightColour = 'G';

    // default values for player settings
    playerSettings->playerArrSize = 2;
    playerSettings->playerSettings = (struct player_t**)malloc(playerSettings->playerArrSize * sizeof(struct player_t*));

    // TODO: add a seperate file for console colors -> like a hashmap with functions

    // player 1
    playerSettings->playerSettings[0] = (struct player_t*)malloc(sizeof(struct player_t));
    playerSettings->playerSettings[0]->number = 1;
    playerSettings->playerSettings[0]->colour = 'R';
    playerSettings->playerSettings[0]->symbol = 'X';

    // player 2
    playerSettings->playerSettings[1] = (struct player_t*)malloc(sizeof(struct player_t));
    playerSettings->playerSettings[1]->number = 2;
    playerSettings->playerSettings[1]->colour = 'B';
    playerSettings->playerSettings[1]->symbol = 'O';

    // default values for file settings
    strcpy(fileSettings.logFileName, "log.txt\0");
    fileSettings.logFileNameLen = strlen(fileSettings.logFileName);
    strcpy(fileSettings.settingsFileName, "settings.bin\0");
    fileSettings.settingsFileNameLen = strlen(fileSettings.settingsFileName);
    strcpy(fileSettings.statsFileName, "stats.bin\0");
    fileSettings.statsFileNameLen = strlen(fileSettings.statsFileName);

    // memcpy because gameSettings has const vars and cant assign it
    memcpy(&settings->gameSettings, &gameSettings, sizeof(struct gameSettings_t));
    settings->playerSettings = playerSettings;
    settings->fileSettings = fileSettings;

    return settings;
}

void display_settings_menu(struct settings_t* settings){
    printf("GAME SETTINGS\n\n");
    printf("board rows - %hd\n", settings->gameSettings.boardRows);
    printf("board columns - %hd\n\n", settings->gameSettings.boardColumns);
    printf("max rows - %hd\n", settings->gameSettings.maxRows);
    printf("max columns - %hd\n\n", settings->gameSettings.maxColumns);
    printf("against bot - %hd\n", settings->gameSettings.againstBot);
    printf("bot depth - %hd\n\n", settings->gameSettings.botDepth);
    printf("multiplayer mode - %hd\n\n", settings->gameSettings.multiplayerMode);
    printf("player amount - %hd\n\n\n\n", settings->gameSettings.playerAmount);


    printf("PLAYER SETTINGS\n\n");

    printf("player arr size - %hd\n\n", settings->playerSettings->playerArrSize);

    for(short i =0; i<settings->playerSettings->playerArrSize; i++){
        printf("player %hd\n", settings->playerSettings->playerSettings[i]->number);
        printf("player colour - %c\n", settings->playerSettings->playerSettings[i]->colour);
        printf("player symbol - %c\n\n", settings->playerSettings->playerSettings[i]->symbol);
    }
    printf("\n\n");

    printf("FILE SETTINGS\n\n");
    printf("log filename - %s\n", settings->fileSettings.logFileName);
    printf("settings filename - %s\n", settings->fileSettings.settingsFileName);
    printf("stats filename - %s\n", settings->fileSettings.statsFileName);
}


/* -------------------------------------------------------------------------- */
