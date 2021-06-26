SCRIPT_DIR=$(dirname "${BASH_SOURCE[0]}")
PATH_TO_SRC="../src"
MAIN_FILE_NAME="main.c"
OUT_FILE_NAME="temp"
COMPILER="gcc -Wall -pedantic -std=c11 "
EXECUTE_OUT="./$OUT_FILE_NAME"
DELETE_OUT="rm $OUT_FILE_NAME"
PATH_TO_RES="../res"
ERROR_LOG_FILE_NAME="errors.log"

 # Navigate to correct dir
cd $SCRIPT_DIR # cd to dir of the script
cd $PATH_TO_SRC # cd to src - game doesnt work when ran from a different directory

# log time for error log
{
    echo ""
    echo ""
    echo ""
    echo "$(date)"
    echo ""
} >> "$PATH_TO_RES/$ERROR_LOG_FILE_NAME"


 # Compile and run file

{
    eval "$COMPILER$MAIN_FILE_NAME -o $OUT_FILE_NAME"
    $EXECUTE_OUT
    $DELETE_OUT
} 3>&2 2>&1 1>&3 | tee -a "$PATH_TO_RES/$ERROR_LOG_FILE_NAME" # Redirect stderr (for error handling) while preserving stdout

# Dont have to go back to original dir because the script is ran in a subshell and is returned to origingal one when it ends
