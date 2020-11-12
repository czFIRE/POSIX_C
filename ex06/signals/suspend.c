#define _GNU_SOURCE

#include <errno.h>
#include <signal.h>
#include <stdlib.h>

#include <error.h>
#include <syslog.h>
#include <unistd.h>

#define UNUSED(VAR) ((void) (VAR))
volatile sig_atomic_t sig_received;

void handler(int signum)
{
    UNUSED(signum);
    sig_received = 1;
}

int main(int argc, char *argv[])
{
    UNUSED(argc);

    openlog(argv[0], LOG_CONS|LOG_PERROR|LOG_PID, LOG_USER);
    syslog(LOG_INFO, "START %d", getpid());

    sigset_t sig_intset;
    sigset_t sig_oldset;

    sigemptyset(&sig_intset);
    sigaddset(&sig_intset, SIGINT);

    if (sigprocmask(SIG_BLOCK, &sig_intset, &sig_oldset) == -1)
        error(EXIT_FAILURE, errno, "sigprocmask");

    struct sigaction action = {
        .sa_handler = handler,
    };

    if (sigaction(SIGINT, &action, NULL) == -1)
        error(EXIT_FAILURE, errno, "sigaction(SIGINT)");

    while (1) {
        sigsuspend(&sig_oldset);

        if (sig_received)
            syslog(LOG_INFO, "SIGINT received");

        sleep(1);
        sig_received = 0;
    }

    closelog();
    return EXIT_SUCCESS;
}
