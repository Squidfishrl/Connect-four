
# redirect stdout and stderr to /dev/null and then get pwd
# get dir of the script
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"
PATH_TO_SRC="../src"
MAIN_FILE_NAME="main.c"
OUT_FILE_NAME="temp.o"
COMPILER="gcc -Wall -pedantic -std=c11 "
EXECUTE_OUT="./$OUT_FILE_NAME"
DELETE_OUT="rm $OUT_FILE_NAME"

# if [ -z ${$1+x} ] # take absolute math
# then

cd $SCRIPT_DIR # cd to dir of the script
cd $PATH_TO_SRC # cd to src - game doesnt work when ran from a different directory
eval "$COMPILER$MAIN_FILE_NAME -o $OUT_FILE_NAME"
$EXECUTE_OUT
$DELETE_OUT
# Dont have to go back to original dir cuz the script is ran in a subshell and is returned to oriwingal wd when it ends
