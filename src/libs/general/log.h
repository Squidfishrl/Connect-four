/* IMPORTED LIBRARIES */

#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

/* -------------------------------------------------------------------------- */


/* STRUCTURE DEFINITIONS */



/* -------------------------------------------------------------------------- */


/* FUNCTION DECLARATIONS */

// writes to stderr which is then redirected to file
bool log_stderr(bool debugMode, short level, char* msg); //returns debug

/* -------------------------------------------------------------------------- */


/* MAIN */



/* -------------------------------------------------------------------------- */


/* FUNCTION DEFINITIONS */

bool log_stderr(bool debugMode, short level, char* msg){
    // every time debugMode is 1 -> change debug to true/false
    static bool debug = false;

    if(debugMode == 1){
        debug = debug ? 0 : 1;
    }
    /*
    0 - debug
    1 - info
    2 - warning
    3 - error
    */
    char* logType;

    // assign level
    switch (level){
        case 0:
            if(debug == 1){
                logType = "DEBUG";
                break;
            }else{
                return debug;
            }
        case 1:
            logType = "INFO";
            break;

        case 2:
            logType = "WARNING";
            break;
        case 3:
            logType = "ERROR";
            break;
        default:
            log_stderr(0, 2, "Invaild log level");
            return debug;
    }

    time_t now; // seconds in ld
    time(&now);

    // char* dateTime = ctime(&now);
    /* for some ANNOYING reason,
    whenever I call ctime/gmtime/localtime/asctime..
    it messes up stdout(I assume) and all future prints have trailing characers
    flushing std i/o doesnt change anything. No one else seems to have this issue either
    */

    fprintf(stderr, "[%s] (%ld) %s\n", logType, now, msg);
    return debug;
}

/* -------------------------------------------------------------------------- */
