#include "strategy.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct guess {
    long min;
    long max;
    unsigned char tries;
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

    srand(difftime(time(NULL), clock()));
    return (rand() % (max - min)) + min;
}

void notify(void *self, int sign)
{
    struct guess *lib = (struct guess *)self;

    // earlier form of shutting down, shouldn't be needed, but time is tricky
    if (lib->tries > 0) {
        fprintf(stderr, "Oracle needs to take a time off, his inner eye is sore. Try again later.\n");
        exit(EXIT_FAILURE);
    }

    lib->tries++;
}