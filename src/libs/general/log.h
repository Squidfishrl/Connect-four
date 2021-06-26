/* IMPORTED LIBRARIES */

#include <stdio.h>
#include <time.h>
#include <string.h>

/* -------------------------------------------------------------------------- */


/* STRUCTURE DEFINITIONS */



/* -------------------------------------------------------------------------- */


/* FUNCTION DECLARATIONS */

// writes to stderr which is then redirected to file
void log_stderr(short level, char* msg);

/* -------------------------------------------------------------------------- */


/* MAIN */



/* -------------------------------------------------------------------------- */


/* FUNCTION DEFINITIONS */

void log_stderr(short level, char* msg){
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
            logType = "DEBUG";
            break;
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
            log_stderr(2, "Invaild log level");
            return;
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
}

/* -------------------------------------------------------------------------- */
