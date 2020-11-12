#define _GNU_SOURCE

#include <signal.h>
#include <stdlib.h>

#include <syslog.h>
#include <unistd.h>

#define UNUSED(VAR) ((void) (VAR))

int main(int argc, char *argv[])
{
    UNUSED(argc);

    openlog(argv[0], LOG_CONS|LOG_PERROR|LOG_PID, LOG_USER);
    syslog(LOG_INFO, "START %d", getpid());

    sigset_t sigint_set;
    sigemptyset(&sigint_set);
    sigaddset(&sigint_set, SIGINT);
    sigprocmask(SIG_BLOCK, &sigint_set, NULL);

    siginfo_t siginfo;
    while (1) {
        sigwaitinfo(&sigint_set, &siginfo);
        syslog(LOG_INFO, "SIGINT received");

        switch (siginfo.si_code) {
        case SI_USER:
            syslog(LOG_INFO, "kill: pid=%d uid=%d", siginfo.si_pid, siginfo.si_uid);
            break;
        case SI_QUEUE:
            syslog(LOG_INFO, "sigqueue: pid=%d uid=%d value=0x%x", siginfo.si_pid, siginfo.si_uid, siginfo.si_int);
            break;
        case SI_KERNEL:
            syslog(LOG_INFO, "kernel");
            break;
        default:
            syslog(LOG_INFO, "other source");
        }
    }

    closelog();
    return EXIT_SUCCESS;
}
