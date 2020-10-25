#include "strategy.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define UNUSED(...) (void)(__VA_ARGS__)

struct guess {
    long min;
    long max;
    unsigned int tries;
};

void *init(long min, long max)
{
    struct guess *lib = malloc(sizeof(struct guess));

    if (!lib) {
        return 0;
    }

    lib->min = min;
    lib->max = max;
    lib->tries = 0;

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

    if (lib->tries < 2) {
        srand(time(NULL) - lib->tries);
    }
    return (rand() % (max - min)) + min;
}

void notify(void *self, int sign)
{
    UNUSED(sign);
    struct guess *lib = (struct guess *)self;
    lib->tries++;
}