#include "lib.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    if (argc != 2) {
        fprintf(stderr, "Wrong amount of arguments\n");
        return 1;
    }

    int input_number = strtol(argv[1], NULL, 10);

    if (input_number < 0) {
        fprintf(stderr, "Negative number on input\n");
        return 1;
    }

    printf("%d\n", fact(input_number));
    return 0;
}