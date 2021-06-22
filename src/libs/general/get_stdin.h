/* IMPORTED LIBRARIES */

#include <stdio.h>
#include <stdbool.h>

/* -------------------------------------------------------------------------- */


/* STRUCTURE DEFINITIONS */



/* -------------------------------------------------------------------------- */


/* FUNCTION DECLARATIONS */

bool get_short(void* var, short minVal, short maxVal, char* errmsg); // returns if changes were made -> escape to cancel
bool get_short_from_char(void* var, short minVal, short maxVal, char* errmsg); // 0-9 and A-Z
bool get_bool(void* var, char* errmsg);
char clear_stdin(); // returns the first char it scnaned

/* -------------------------------------------------------------------------- */


/* FUNCTION DEFINITIONS */

bool get_short_from_char(void* var, short minVal, short maxVal, char* errmsg){

    while(true){

        // clear stdin with fflush (not requiring input)

        *(short*)var = getchar();
        if(clear_stdin() != '\n'){

            // print error msg (multiple chars entered)

            if(maxVal < 10){
                printf("%s (%hd-%hd): ", errmsg, minVal, maxVal);
            }else{
                printf("%s (%hd-%c): ", errmsg, minVal, maxVal-10+'A');
            }
            continue;
        }

        // if escape is first key that was pressed return without changing var
        if(*(short*)var == 27){
            return false;
        }

        // 0-9
        if(*(short*)var >= '0' && *(short*)var <= '9'){
            *(short*)var = *(short*)var - '0';

            if(*(short*)var >= minVal && *(short*)var <= maxVal){
                break;
            }
        }

        // 10 - 35
        if(*(short*)var >= 'A' && *(short*)var <= 'Z'){
            *(short*)var = *(short*)var - 'A' + 10;

            if(*(short*)var >= minVal && *(short*)var <= maxVal){
                break;
            }
        }

        // 10-35
        if(*(short*)var >= 'a' && *(short*)var <= 'z'){
            *(short*)var = *(short*)var - 'a' + 10;

            if(*(short*)var >= minVal && *(short*)var <= maxVal){
                break;
            }
        }

        // print error msg (char entered is invalid)
        if(maxVal < 10){
            printf("%s (%hd-%hd): ", errmsg, minVal, maxVal);
        }else{
            printf("%s (%hd-%c): ", errmsg, minVal, maxVal-10+'A');
        }

    }

    return true;
}

char clear_stdin(){

    char inputBufferRead = getchar();
    char returnVal = inputBufferRead;

    while((inputBufferRead != EOF && inputBufferRead != '\n')){
        inputBufferRead = getchar();
    }

    return returnVal;
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
        clear_stdin();

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
        clear_stdin();

        // take new input
        printf("%s (%hd-%hd): ",errmsg, 0, 1);
        scanf("%hd", &holdVal);
        inputBufferRead = getchar();
    }

    *(bool*)var = holdVal;
    return true;
}

/* -------------------------------------------------------------------------- */
