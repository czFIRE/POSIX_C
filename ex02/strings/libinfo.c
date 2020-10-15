#include "libfilter.h"
#include <stdio.h>

#define VERSION 1

__attribute__((constructor))
static void library_init(void) {
    printf("INFO lib: \n \
version: %d  \n \
author name: czFIRE \n"
            , VERSION);
}

__attribute__((destructor))
static void library_destroy(void) {
    printf("I am dying\n");
}

// identity
void filter(char *str)
{
    return;
}