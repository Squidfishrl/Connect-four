/* IMPORTED LIBRARIES */

#include <stdio.h>
#include <stdbool.h>

/* -------------------------------------------------------------------------- */


/* STRUCTURE DEFINITIONS */



/* -------------------------------------------------------------------------- */


/* FUNCTION DECLARATIONS */

bool get_short(void* var, short minVal, short maxVal, char* errmsg); // returns if changes were made -> escape to cancel
bool get_bool(void* var, char* errmsg);
void clear_stdin();

/* -------------------------------------------------------------------------- */


/* MAIN */



/* -------------------------------------------------------------------------- */


/* FUNCTION DEFINITIONS */

void clear_stdin(){
    char inputBufferRead = getchar();

    while((inputBufferRead != EOF && inputBufferRead != '\n')){
        inputBufferRead = getchar();
    }
}

bool get_short(void* var, short minVal, short maxVal, char* errmsg){

    scanf("%hd", (short*)var);
    char inputBufferRead = getchar();

    while((inputBufferRead != EOF && inputBufferRead != '\n') || (*(short*)var < minVal || *(short*)var > maxVal)){

        // if escape is first key that was pressed return without changing var
        if(inputBufferRead == 27){
            return false;
        }

        // clear rest of stdin buffer so that msg isnt printed twice when a str is inputed]
        while((inputBufferRead != EOF && inputBufferRead != '\n')){
            inputBufferRead = getchar();
        }

        // take new input
        printf("%s (%hd-%hd): ", errmsg, minVal, maxVal);
        scanf("%hd", (short*)var);
        inputBufferRead = getchar();
    }

    return true;
}

bool get_bool(void* var, char* errmsg){
    short holdVal;

    scanf("%hd", &holdVal);
    char inputBufferRead = getchar();

    while((inputBufferRead != EOF && inputBufferRead != '\n') || (holdVal < 0 || holdVal > 1)){

        // if escape is first key that was pressed return without changing var
        if(inputBufferRead == 27){
            return false;
        }

        // clear rest of stdin buffer [so that msg isnt printed twice when a str is inputed]
        while((inputBufferRead != EOF && inputBufferRead != '\n')){
            inputBufferRead = getchar();
        }

        // take new input
        printf("%s (%hd-%hd): ",errmsg, 0, 1);
        scanf("%hd", &holdVal);
        inputBufferRead = getchar();
    }

    *(bool*)var = holdVal;
    return true;
}

/* -------------------------------------------------------------------------- */
