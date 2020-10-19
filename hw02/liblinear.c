#include "strategy.h"

#include <stdbool.h>
#include <stdlib.h>

struct guess {
    long min;
    long max;
    bool state;
};

void *init(long min, long max)
{
    struct guess *lib = malloc(sizeof(struct guess));

    if (!lib) {
        return 0;
    }

    lib->min = min;
    lib->max = max;
    lib->state = false;

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

    if (lib->state) {
        return lib->min;
    }
    return lib->max;
}

void notify(void *self, int sign)
{
    struct guess *lib = (struct guess *)self;

    lib->state = !(lib->state);

    if (!lib->state) {
        lib->min++;
    } else {
        lib->max--;
    }
}