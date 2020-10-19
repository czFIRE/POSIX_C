#include "strategy.h"

#include <stdio.h>
#include <stdlib.h>

struct guess {
    long min;
    long max;
};

void *init(long min, long max)
{
    struct guess *lib = malloc(sizeof(struct guess));

    if (!lib) {
        return 0;
    }

    lib->min = min;
    lib->max = max;

    return lib;
}

void destroy(void *self)
{
    //
    free(self);
}

long guess(void *self)
{
    struct guess *lib = (struct guess *)self;
    int max = lib->max;
    int min = lib->min;
    return (min + max) / 2;
}

void notify(void *self, int sign)
{
    struct guess *lib = (struct guess *)self;
    int max = lib->max;
    int min = lib->min;

    switch (sign) {
    case 1:
        lib->min = (min + max) / 2;
        break;

    case -1:
        lib->max = (min + max) / 2;
        break;

    default:
        fprintf(stderr, "Internal error: wrong parametr in notify()");
        exit(EXIT_FAILURE);
    }
}