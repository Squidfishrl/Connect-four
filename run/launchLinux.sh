compile="gcc -Wall -pedantic -std=c11 ../src/main.c -o temp.out"
run="./temp.out"
delOut="rm temp.out"

$compile
$run
$delOut
