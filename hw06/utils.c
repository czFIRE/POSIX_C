#define _GNU_SOURCE
#include "utils.h"
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <error.h>
#include <signal.h>
#include <syslog.h>
#include <unistd.h>

void signal_handler(int sig, siginfo_t *siginf, void *context);

const struct signal_value signals_to_log[] = {
    {"SIGINT", SIGINT},   {"SIGQUIT", SIGQUIT}, {"SIGTERM", SIGTERM},
    {"SIGUSR1", SIGUSR1}, {"SIGUSR2", SIGUSR2}, {"SIGCONT", SIGCONT},
};

#define SIGNAL_NUM (int)(sizeof(signals_to_log) / sizeof(struct signal_value))

void help(void)
{
    printf("Logger of signals\n"
           "Usage:\n"
           "eh, am lazy to do this, but I would just copy the assigment\n");
}

void logger(struct program_options options)
{
    // printf("PID: %d\n", getpid());
    if (options.daemon) {
        if (daemon(1, 1)) {
            error(EXIT_FAILURE, errno, "daemon opennig failed");
        }
    }

    openlog(NULL, options.error ? LOG_PERROR : LOG_PID,
            options.daemon ? LOG_DAEMON : LOG_USER);
    syslog(LOG_INFO, "PID: %d", getpid());

    sigset_t sig_intset;
    sigset_t sig_oldset;

    sigemptyset(&sig_intset);

    // sizeof works for hardcoded lists
    for (size_t i = 0; i < SIGNAL_NUM; i++) {
        sigaddset(&sig_intset, signals_to_log[i].sig_val);
        syslog(LOG_DEBUG, "Added signal %s to mask",
               signals_to_log[i].sig_name);
    }

    if (sigprocmask(SIG_BLOCK, &sig_intset, &sig_oldset) == -1)
        error(EXIT_FAILURE, errno, "sigprocmask");

    struct sigaction sig_handler = {
        .sa_sigaction = &signal_handler,
        .sa_flags = SA_RESTART | SA_SIGINFO,
        .sa_mask = sig_intset,
    };

    for (size_t i = 0; i < SIGNAL_NUM; i++) {
        if (sigaction(signals_to_log[i].sig_val, &sig_handler, NULL))
            error(EXIT_FAILURE, errno, "can't set handler for %s",
                  signals_to_log[i].sig_name);
        syslog(LOG_DEBUG, "Added signal %s to handler",
               signals_to_log[i].sig_name);
    }

    // maybe init statistics here?

    syslog(LOG_INFO, "Initalization successful");
    while (1) {
        sigsuspend(&sig_oldset);
        // return;
    }
}

void signal_handler(int sig, siginfo_t *siginf, void *context)
{
    UNUSED(siginf);
    UNUSED(context);

    syslog(LOG_INFO, "Got signal: %d\n", sig);

    if ((sig == SIGTERM) || (sig == SIGINT)) {
        // do I really want to call exit?
        syslog(LOG_INFO, "shutting down");
        exit(EXIT_SUCCESS);
    }
}