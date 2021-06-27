#include <stdio.h>
#include "./libs/general/get_stdin.h"


int main(int argc, char const *argv[]) {

    char filename[37];

    printf("enter input: ");
    get_validated_str(filename, 1, 30, "\nenter input: ");

    printf("%s", filename);
    return 0;
}
