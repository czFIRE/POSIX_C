#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <error.h>
#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define USRSIGADD SIGUSR1
#define USRSIGGET SIGUSR2

void server(void);
void get(pid_t PID);
void add(pid_t PID, int number);

#define UNUSED(VAR) ((void)(VAR))
volatile sig_atomic_t counter = 0;

void usrsigadd(int sig, siginfo_t *siginf, void *context)
{
    UNUSED(sig);
    UNUSED(context);
    int value = siginf->si_int;

    printf("add: %d\n", value);
    counter += value;
}

void usrsigget(int sig, siginfo_t *siginf, void *context)
{
    UNUSED(sig);
    UNUSED(context);
    UNUSED(siginf);
    printf("get: %d\n", counter);
}

int main(int argc, char *argv[])
{
    switch (argc) {
    // server
    case 2:
        server();
        break;

    // get
    case 3: {
        // should do proper parsing
        pid_t pid = strtol(argv[2], NULL, 10);
        get(pid);
        break;
    }

    // add -> my add requires argument cause I am lazy to think about parsing
    case 4: {
        pid_t pid = strtol(argv[2], NULL, 10);
        int number = strtol(argv[3], NULL, 10);
        add(pid, number);
        break;
    }

    default:
        fprintf(stderr, "wrong amount of args");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

void server(void)
{
    sigset_t sig_intset;
    sigset_t sig_oldset;

    sigemptyset(&sig_intset);
    sigaddset(&sig_intset, USRSIGADD);
    sigaddset(&sig_intset, USRSIGGET);

    if (sigprocmask(SIG_BLOCK, &sig_intset, &sig_oldset) == -1)
        error(EXIT_FAILURE, errno, "sigprocmask");

    struct sigaction usrsigget_handler = {
        .sa_sigaction = &usrsigget,
        .sa_flags = SA_RESTART,
    };

    if (sigaction(USRSIGGET, &usrsigget_handler, NULL) != 0)
        error(EXIT_FAILURE, errno, "cannot setup handler for GET");

    struct sigaction usrsigadd_handler = {
        .sa_sigaction = &usrsigadd,
        .sa_flags = SA_RESTART,
    };

    if (sigaction(USRSIGADD, &usrsigadd_handler, NULL) != 0)
        error(EXIT_FAILURE, errno, "cannot setup handler for ADD");

    printf("Server pid: %d\n", getpid());

    // not even this gets executed :thinking:
    while (1) {
        sigsuspend(&sig_oldset);

        // pause();
    }
}

void get(pid_t PID)
{
    printf("Calling get: PID = %d\n", PID);
    union sigval num = {.sival_ptr = NULL};
    if (sigqueue(PID, USRSIGGET, num) == -1)
        error(EXIT_FAILURE, errno, "sigqueue");
}

void add(pid_t PID, int number)
{
    printf("Calling get: PID = %d, number = %d\n", PID, number);
    union sigval num = {.sival_int = number};
    // num.sival_int = number;
    if (sigqueue(PID, USRSIGADD, num) == -1)
        error(EXIT_FAILURE, errno, "sigqueue");
}