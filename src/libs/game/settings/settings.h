/* IMPORTED LIBRARIES */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>

#include "../../general/colours.h"
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

struct settings_t* init_settings(char* fileName, struct dict_t* colourDict);
struct settings_t* define_settings(struct dict_t* colourDict); // create default settings
bool read_settings(char* fileName, struct settings_t* settings);
bool change_setting(struct settings_t* settings, short settingNO); // TODO: useless - remove
bool write_settings(char* fileName, struct settings_t* settings);
void free_settings(struct settings_t* settings); // TODO:
void add_player(struct playerSettings_t* settings, struct dict_t* colourDict);

void display_settings_menu(struct settings_t* settings, struct dict_t* colourDict); // NOTE: could add filename depending what I do in it
bool display_game_settings_menu(struct settings_t* settings, struct dict_t* colourDict); // returns if changes were made
bool display_player_settings_menu(struct settings_t* settings, struct dict_t* colourDict);

/* -------------------------------------------------------------------------- */


/* FUNCTION DEFINITIONS */

void add_player(struct playerSettings_t* settings, struct dict_t* colourDict){

    // allocate space for a new player
    settings->playerSettings = (struct player_t**)realloc(settings->playerSettings, ++settings->playerArrSize * sizeof(struct player_t*) );

    // allocate new player
    settings->playerSettings[settings->playerArrSize-1] = (struct player_t*)malloc(sizeof(struct player_t));

    //assign number
    settings->playerSettings[settings->playerArrSize-1]->number = settings->playerArrSize;

    /* assign random colour */

    // colour key
    settings->playerSettings[settings->playerArrSize-1]->colour = fetch_random_colour(colourDict);
    //colour code
    strcpy(settings->playerSettings[settings->playerArrSize-1]->colourCode, binary_search_dict(settings->playerSettings[settings->playerArrSize-1]->colour, colourDict));
    // colour code len
    settings->playerSettings[settings->playerArrSize-1]->colourCodeLen = strlen(settings->playerSettings[settings->playerArrSize-1]->colourCode);

    /* assign symbol */
    settings->playerSettings[settings->playerArrSize-1]->symbol = '1' + settings->playerArrSize - 1;
}

void free_settings(struct settings_t* settings){

    // free player settings[] players
    for(short i = 0; i < settings->playerSettings->playerArrSize; i++){
        free(settings->playerSettings->playerSettings[i]);
    }
    // free player settings arr
    free(settings->playerSettings->playerSettings);
    // free player settings
    free(settings->playerSettings);

    // free settigs
    free(settings);

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
    char editedFileName[strlen(fileName) + strlen("../res/bin/")];
    sprintf(editedFileName, "../res/bin/%s", fileName);

    // file exists
    if(access(editedFileName, F_OK) == 0){


        settings = (struct settings_t*)malloc(sizeof(struct settings_t));

        // read file to settings
        if(!read_settings(editedFileName, settings)){
            printf("Failed to create or open settings file!\n");
            return NULL;
        }
    }else{
        //file doesn't exist

        settings = define_settings(colourDict);
        if(!write_settings(editedFileName, settings)){
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
    gameSettings.boardRows = 10;
    gameSettings.boardColumns = 7;
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
    // TODO: maybe dont store ,,/,,/ .. right here
    strcpy(fileSettings.logFileName, "log.txt\0");
    fileSettings.logFileNameLen = strlen(fileSettings.logFileName);

    // settings
    // TODO: maybe dont store ,,/,,/ .. right here
    strcpy(fileSettings.settingsFileName, "settings.bin\0");
    fileSettings.settingsFileNameLen = strlen(fileSettings.settingsFileName);

    // TODO: maybe dont store ,,/,,/ .. right here
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
    bool exitCheck = false;
    char settingsFileName[settings->fileSettings.settingsFileNameLen + strlen("../res/bin/")];
    sprintf(settingsFileName, "../res/bin/%s", settings->fileSettings.settingsFileName);
    // colour vars
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


        printf(" \n go to: ");
        // get input and check if esc key was pressed
        exitCheck = !get_short(&settingsNO, 1, 4, errmsg);

        // exit condition
        if(exitCheck == true || settingsNO == 4){
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
                // if changes were made aknowledge it
                changedSettings = true;
            };
        }else if(settingsNO == 2){
            if(display_player_settings_menu(settings, colourDict)){
                changedSettings = true;
            }
        }else{
            // TODO: file settings
        }

    }

    // return;
}

bool display_game_settings_menu(struct settings_t* settings, struct dict_t* colourDict){

    short settingsNO;
    bool exitCheck = false;

    // colour vars
    const char *flash, *white, *def, *yellow, *heavy;
    flash = binary_search_dict('F', colourDict);
    white = binary_search_dict('W', colourDict);
    def = binary_search_dict('D', colourDict);
    heavy = binary_search_dict('H', colourDict);
    yellow = binary_search_dict('Y', colourDict);
    bool settingsChange = false;

    // stdin err msges
    char errmsg1[] = {" \n Invalid input! Try again\n go to"};
    char errmsg2[] = {" \n Invalid input! Try again\n"};
    strcat(errmsg2, heavy);
    strcat(errmsg2, white);
    strcat(errmsg2, " NEW VALUE: ");
    strcat(errmsg2, def);

    // arr to hold all game settings info
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
        {
            .name = "multiplayer",
            .description = "[WIP]Allows playing online via socket connections.",
            .value = &settings->gameSettings.multiplayer,
            .minValue = 0,
            .maxValue = 1
        },
        {
            .name = "player_amount",
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

        // print all setting name-value from settingsArr
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

        system("clear");

        // exit condition
        if(settingsNO == i+1 || exitCheck == true){
            break;
        }

        // print additional info about settings
        printf("%s%s CONNECT FOUR %s", heavy, yellow, def);
        printf("%s (game-settings-%s) %s\n\n", yellow, settingsArr[settingsNO-1].name, def);

        printf("%s%s NAME: %s %s\n", heavy, white, def, settingsArr[settingsNO - 1].name);
        printf("%s%s DESCRIPTION: %s %s\n", heavy, white, def, settingsArr[settingsNO - 1].description);
        printf("%s%s VALUE RANGE: %s (%hd-%hd)\n", heavy, white, def, settingsArr[settingsNO - 1].minValue, settingsArr[settingsNO - 1].maxValue);

        // special case for char val setting
        if(strcmp(settingsArr[settingsNO - 1].name, "win_highlight_colour") == 0){
            print_colour_dict(colourDict);
            printf("\n\n%s%s CURRENT VALUE:%s '%c'\n", heavy, white, def, *(char*)settingsArr[settingsNO - 1].value);
        }else{
            printf("%s%s CURRENT VALUE: %s %hd\n", heavy, white, def, *(short*)settingsArr[settingsNO - 1].value);
        }

        printf("\n%s%s NEW VALUE: %s", heavy, white, def);

        // if new value is assigned change settingsChange to true
        if(settingsArr[settingsNO - 1].maxValue == 1 && settingsArr[settingsNO - 1].minValue == 0){

            if(get_bool(settingsArr[settingsNO-1].value, errmsg2)){
                settingsChange = true;
            }
        }else{
            if(get_short(settingsArr[settingsNO-1].value, settingsArr[settingsNO-1].minValue, settingsArr[settingsNO-1].maxValue, errmsg2)){
                settingsChange = true;

                // special case for char val setting
                if(strcmp(settingsArr[settingsNO-1].name, "win_highlight_colour") == 0){
                    *(char*)settingsArr[settingsNO-1].value = colourDict->nodeArr[*(short*)settingsArr[settingsNO-1].value-1].key;

                // case for changing player amount
                }else if(strcmp(settingsArr[settingsNO-1].name, "player_amount") == 0){

                    // check if player amount > playerArrSize
                    if(*(int*)settingsArr[settingsNO-1].value > settings->playerSettings->playerArrSize){
                        printf("player arr size - %hd", settings->playerSettings->playerArrSize);
                        printf("\n ERROR - Not enough players defined. Add more via the player settings menu. (value set to 2)");
                        *(int*)settingsArr[settingsNO-1].value = 2;
                        getchar(); // wait for user input
                    }
                }
            }

        }
    }
    return settingsChange;
}


bool display_player_settings_menu(struct settings_t* settings, struct dict_t* colourDict){

    short settingsNO, settingsNO2, settingsNO3;
    bool exitCheck, exitCheck2, exitCheck3, settingsChange;
    exitCheck = exitCheck2 = exitCheck3 = settingsChange = false;

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
            break;
        }else if(settingsNO == i+1){ // add player
            if(settings->playerSettings->playerArrSize >= 9){
                printf("ERROR - already reached max player amount");
                getchar();
            }else{
                add_player(settings->playerSettings, colourDict);
                settingsChange = true;
            }

        }else{
            // choose which player setting u want to change (colour/symbol)
            while(1){

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
                        system("clear");

                        printf("%s%s CONNECT FOUR %s", heavy, yellow, def);
                        printf("%s (player_%hd-settings-symbol_colour) %s\n\n", yellow, settingsNO, def);

                        print_colour_dict(colourDict);

                        printf("\n\n choose colourNO: ");
                        exitCheck3 = !get_short(&settingsNO3, 1, colourDict->currentSize, errmsg2);

                        if(exitCheck3 == true){
                            // exit to player_n-settings
                            continue;
                        }else{



                            settings->playerSettings->playerSettings[settingsNO - 1]->colour = colourDict->nodeArr[settingsNO3 - 1].key;
                            strcpy(settings->playerSettings->playerSettings[settingsNO - 1]->colourCode, colourDict->nodeArr[settingsNO3 - 1].value);
                            settings->playerSettings->playerSettings[settingsNO - 1]->colourCodeLen = strlen(settings->playerSettings->playerSettings[settingsNO2 - 1]->colourCode);

                            settingsChange = true;

                            // exit to player-settings
                            break;
                        }

                    }else{ // == 2

                        system("clear");
                        printf("%s%s CONNECT FOUR %s", heavy, yellow, def);
                        printf("%s (player_%hd-settings-symbol_letter) %s\n\n", yellow, settingsNO, def);

                        printf("\n\n choose symbol: ");
                        settings->playerSettings->playerSettings[settingsNO-1]->symbol = getchar();

                        // gets only the first character of msg -> eat the rest if any
                        clear_stdin();


                        settingsChange = true;

                        // exit to player-settings
                        break;
                    }
                }
            }
        }
    }

    return settingsChange;
}

/* -------------------------------------------------------------------------- */
