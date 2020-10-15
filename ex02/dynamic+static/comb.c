#include "num.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "Wrong amount of arguments\n");
        return 1;
    }

    // should check with endpointer and errno, but am lazy
    int upper = strtol(argv[1], NULL, 10);
    int lower = strtol(argv[2], NULL, 10);

    if (upper < 0 || lower < 0) {
        fprintf(stderr, "Negative number on input\n");
        return 1;
    }

    printf("%d\n", comb(upper, lower));
    return 0;
}