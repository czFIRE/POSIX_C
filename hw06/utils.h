#ifndef SIGLOGD_UTILS_H
#define SIGLOGD_UTILS_H

#include <stdbool.h>

#define UNUSED(VAR) ((void)(VAR))

struct signal_value {
    char *sig_name;
    int sig_val;
};

struct program_options {
    bool daemon;
    bool error;
    unsigned int interval;
};

void logger(struct program_options options);
void log_signal(int sig);
void help(void);

#endif