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

    char logMsg[35];

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
        sprintf(logMsg, "Invalid input '%c' - Out of bouds", *(char*)var);
        log_stderr(0, 2, logMsg);

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

    while((inputBufferRead != '\n')){
        inputBufferRead = getchar();
    }

    return returnVal;
}

bool get_short(void* var, short minVal, short maxVal, char* errmsg){

    short holdVal;
    char logMsg[35];
    char inputBufferRead;

    do{
        log_stderr(0, 0, "Getting user input (short from short)");
        scanf("%hd", &holdVal);
        inputBufferRead = getchar();

        // if escape is first key that was pressed return without changing holdVal
        if(inputBufferRead == 27){
            log_stderr(0, 0, "Exiting input fetch (ESC pressed)");
            clear_stdin();
            return false;
        }

        if(inputBufferRead != '\n'){
            if(clear_stdin() != '\n'){
                // print error msg (multiple chars entered)
                log_stderr(0, 2, "Invalid input - Button spam");

                printf("%s (%hd-%hd): ", errmsg, minVal, maxVal);

                continue;
            }
        }

        if(holdVal >= minVal && holdVal <= maxVal){
            break;
        }else{
            sprintf(logMsg, "Invalid input '%d' - Out of bounds", holdVal);
            log_stderr(0, 2, logMsg);
            printf("%s (%hd-%hd): ", errmsg, minVal, maxVal);
        }

    }while((inputBufferRead != EOF && inputBufferRead != '\n') || (holdVal < minVal || holdVal > maxVal));

    *(short*)var = holdVal;
    return true;
}

bool get_bool(void* var, char* errmsg){

    short holdVal;
    char inputBufferRead;
    char logMsg[35];

    do{
        log_stderr(0, 0, "Getting user input (bool from bool)");
        scanf("%hd", &holdVal);
        inputBufferRead = getchar();

        // if escape is first key that was pressed return without changing holdVal
        if(inputBufferRead == 27){
            log_stderr(0, 0, "Exiting input fetch (ESC pressed)");
            clear_stdin();
            return false;
        }

        if(inputBufferRead != '\n'){
            if(clear_stdin() != '\n'){
                // print error msg (multiple chars entered)
                log_stderr(0, 2, "Invalid input - Button spam");

                printf("%s (0-1): ", errmsg);

                continue;
            }
        }

            if(holdVal == 0 || holdVal == 1){
                break;
            }else{
                sprintf(logMsg, "Invalid input '%d' - Out of bounds", holdVal);
                log_stderr(0, 2, logMsg);
                printf("%s (0-1): ", errmsg);
            }

        }while((inputBufferRead != EOF && inputBufferRead != '\n') || (holdVal < 0 || holdVal > 1));

        *(bool*)var = holdVal;
        return true;
}

/* -------------------------------------------------------------------------- */
