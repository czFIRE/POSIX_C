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
void alarm_handler(int sig, siginfo_t *siginf, void *context);
void print_statistics(void);

struct signal_value signals_to_log[] = {
    {"SIGINT", SIGINT, 0},   {"SIGQUIT", SIGQUIT, 0}, {"SIGTERM", SIGTERM, 0},
    {"SIGUSR1", SIGUSR1, 0}, {"SIGUSR2", SIGUSR2, 0}, {"SIGCONT", SIGCONT, 0},
};

#define SIGNAL_NUM (int)(sizeof(signals_to_log) / sizeof(struct signal_value))

int timer_seconds = 0;
//int process_pid = 0;

void help(void)
{
    printf("Logger of signals\n"
           "Usage:\n"
           "eh, am lazy to do this, but I would just copy the assigment\n");
}

void logger(struct program_options options)
{
    //printf("MIN: %d\n", SIGRTMAX);
    timer_seconds = options.interval;
    if (options.daemon) {
        if (daemon(1, 1)) {
            error(EXIT_FAILURE, errno, "daemon opennig failed");
        }
    }

    openlog(NULL, options.error ? LOG_PERROR : LOG_PID,
            options.daemon ? LOG_DAEMON : LOG_USER);
    syslog(LOG_INFO, "PID: %d", getpid());
    //process_pid = getpid();

    sigset_t sig_intset;
    sigset_t sig_oldset;

    sigemptyset(&sig_intset);

    // sizeof works for hardcoded lists
    for (size_t i = 0; i < SIGNAL_NUM; i++) {
        sigaddset(&sig_intset, signals_to_log[i].sig_val);
        syslog(LOG_DEBUG, "Added signal %s to mask",
               signals_to_log[i].sig_name);
    }

    sigaddset(&sig_intset, SIGALRM);

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

    sig_handler.sa_sigaction = &alarm_handler;
    if (sigaction(SIGALRM, &sig_handler, NULL))
        error(EXIT_FAILURE, errno, "can't set handler for SIGALRM");

    syslog(LOG_INFO, "Initalization successful");
    if (timer_seconds != 0) {
        alarm(timer_seconds);
    }
    while (1) {
        sigsuspend(&sig_oldset);
        // return;
    }
}

void print_statistics(void)
{
    syslog(LOG_INFO, "Signal statistics:");
    for (size_t i = 0; i < SIGNAL_NUM; i++) {
        syslog(LOG_INFO, "%s (%d):\t%ld", signals_to_log[i].sig_name,
               signals_to_log[i].sig_val, signals_to_log[i].statistics);
    }
}

void signal_handler(int sig, siginfo_t *siginf, void *context)
{
    UNUSED(siginf);
    UNUSED(context);

    for (size_t i = 0; i < SIGNAL_NUM; i++) {
        if (sig == signals_to_log[i].sig_val) {
            syslog(LOG_INFO, "Got signal %s (%d)\n", signals_to_log[i].sig_name,
                   sig);
            signals_to_log[i].statistics++;
        }
    }

    if ((sig == SIGTERM) || (sig == SIGINT)) {
        // do I really want to call exit?
        syslog(LOG_INFO, "shutting down");
        print_statistics();
        exit(EXIT_SUCCESS);
    }
}

void alarm_handler(int sig, siginfo_t *siginf, void *context)
{
    UNUSED(sig);
    UNUSED(context);

    //printf("%d %d %d\n", siginf->si_pid, siginf->si_uid, siginf->si_value.sival_int);    
    if (siginf->si_pid != 0) {
        return;
    }

    print_statistics();
    if (timer_seconds != 0) {
        alarm(timer_seconds);
    }
}