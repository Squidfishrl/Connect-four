compile="gcc -Wall -pedantic -std=c11 "
run="./temp.o"
delOut="rm temp.o"
baseDir=$(basename "$PWD") # get base directory (should be Connect_four or run)

if [ "$baseDir" = "Connect_four" ]
then
    pathToMain="src/main.c"
elif [ "$baseDir" = "run" ]
then
    pathToMain="../src/main.c"
else
    echo -e "ERROR - unknown directory\ndirectory name should be Connect_four or Connect_four/run"
    exit 1
fi

eval "$compile$pathToMain -o temp.o"
$run
$delOut

exit 0 # 0 is true in bash incase I forget
