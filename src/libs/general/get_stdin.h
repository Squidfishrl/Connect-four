/* IMPORTED LIBRARIES */

#include <stdio.h>
#include <stdbool.h>
#include "colours.h" // isnt used but is included to avoid log.h redefinition

/* -------------------------------------------------------------------------- */


/* STRUCTURE DEFINITIONS */



/* -------------------------------------------------------------------------- */


/* FUNCTION DECLARATIONS */

bool get_short(void* var, short minVal, short maxVal, char* errmsg); // returns if changes were made -> escape to cancel
bool get_short_from_char(void* var, short minVal, short maxVal, char* errmsg); // 0-9 and A-Z
bool get_bool(void* var, char* errmsg);
char clear_stdin(); // returns the first char it scnaned

/* -------------------------------------------------------------------------- */


/* FUNCTI
  clear_stdin(); // clear stdin incase escape was pressed multiple times to not  instant exitON DEFINITIONS */

bool get_short_from_char(void* var, short minVal, short maxVal, char* errmsg){

    while(true){
        log_stderr(0, 0, "Getting user input (short from char)");
        // clear stdin with fflush (not requiring input)

        // if escape is first key that was pressed return without changing var
        if(*(short*)var == 27){
            log_stderr(0, 0, "Exiting input fetch (ESC pressed)");
            clear_stdin();
            return false;
        }

        *(short*)var = getchar();
        if(clear_stdin() != '\n'){
            // print error msg (multiple chars entered)
            log_stderr(0, 2, "Invalid input - Button spam");

            if(maxVal < 10){
                printf("%s (%hd-%hd): ", errmsg, minVal, maxVal);
            }else{
                printf("%s (%hd-%c): ", errmsg, minVal, maxVal-10+'A');
            }
            continue;
        }

        // 0-9
        if(*(short*)var >= '0' && *(short*)var <= '9'){
            *(short*)var = *(short*)var - '0';

            if(*(short*)var >= minVal && *(short*)var <= maxVal){
                break;
            }
        }

        // 10 - 35
        if(*(short*)var >= 'A' && *(short*)var <= '_'){  // _  - 'A' + 10 =  40 which is max column size while Z would account for 36 columns
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

        log_stderr(0, 2, "Invalid input - Out of bounds");

        if(maxVal < 10){
            printf("%s (%hd-%hd): ", errmsg, minVal, maxVal);
        }else{
            printf("%s (%hd-%c): ", errmsg, minVal, maxVal-10+'A');
        }

    }

    return true;
}

char clear_stdin(){
    log_stderr(0, 0, "Clearing standart input");
    char inputBufferRead = getchar();
    char returnVal = inputBufferRead;

    while((inputBufferRead != EOF && inputBufferRead != '\n')){
        inputBufferRead = getchar();
    }

    return returnVal;
}

bool get_short(void* var, short minVal, short maxVal, char* errmsg){

    log_stderr(0, 0, "Getting user input (short from short)");

    scanf("%hd", (short*)var);
    char inputBufferRead = getchar();

    // if escape is first key that was pressed return without changing var
    if(inputBufferRead == 27){
        log_stderr(0, 0, "Exiting input fetch (ESC pressed)");
        clear_stdin(); // clear stdin incase escape was pressed multiple times to not  instant exit
        return false;
    }


    while((inputBufferRead != EOF && inputBufferRead != '\n') || (*(short*)var < minVal || *(short*)var > maxVal)){
        log_stderr(0, 2, "Invalid input - Out of bounds");


        // clear rest of stdin buffer so that msg isnt printed twice when a str is inputed]
        clear_stdin();

        log_stderr(0, 0, "Getting user input (short from short)");
        // take new input
        printf("%s (%hd-%hd): ", errmsg, minVal, maxVal);
        scanf("%hd", (short*)var);
        inputBufferRead = getchar();
    }

    return true;
}

bool get_bool(void* var, char* errmsg){

    log_stderr(0, 0, "Getting user input (bool from bool)");

    short holdVal;

    scanf("%hd", &holdVal);
    char inputBufferRead = getchar();

    // if escape is first key that was pressed return without changing var
    if(inputBufferRead == 27){
        log_stderr(0, 0, "Exiting input fetch (ESC pressed)");
        clear_stdin();
        return false;
    }

    while((inputBufferRead != EOF && inputBufferRead != '\n') || (holdVal < 0 || holdVal > 1)){
        log_stderr(0, 2, "Invalid input - Out of bounds");

        // clear rest of stdin buffer [so that msg isnt printed twice when a str is inputed]
        clear_stdin();

        log_stderr(0, 0, "Getting user input (short from short)");
        // take new input
        printf("%s (%hd-%hd): ",errmsg, 0, 1);
        scanf("%hd", &holdVal);
        inputBufferRead = getchar();
    }

    *(bool*)var = holdVal;
    return true;
}

/* -------------------------------------------------------------------------- */
