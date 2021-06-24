compile="gcc -Wall -pedantic -std=c11 "
run="./temp.o"
delOut="rm temp.o"
baseDir=$(basename "$PWD") # get base directory (should be Connect_four or run)
mainFile="main.c"

if [ "$baseDir" = "Connect-four" ]
then
    pathToSrc="./src"
elif [ "$baseDir" = "run" ]
then
    pathToSrc="../src"
else
    echo -e "ERROR - unknown directory\ndirectory name should be Connect_four or Connect_four/run"
    exit 1
fi

cd $pathToSrc # game runs only when its ran from ./src
eval "$compile$mainFile -o temp.o"
$run
$delOut
# Dont have to go back to original dir cuz the script is ran in a subshell and is returned to oriwingal wd when it ends

exit 0 # 0 is true in bash incase I forget
