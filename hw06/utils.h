#ifndef SIGLOGD_UTILS_H
#define SIGLOGD_UTILS_H

#include <stdbool.h>

struct program_options {
    bool demon;
    bool error;
    unsigned int interval;
};

void help(void);

#endif