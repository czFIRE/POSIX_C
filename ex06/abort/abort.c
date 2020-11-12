#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

#include <error.h>

#define UNUSED(VAR) \
    (void) (VAR)

void sigabrt_ignore(int sig)
{
    UNUSED(sig);
    fprintf(stderr, "SIGABRT caught, ignoring\n");
}

int main(void)
{
    struct sigaction sigabrt_handler = {
        .sa_handler = &sigabrt_ignore,
        .sa_flags   = SA_RESTART,
    };

    if (sigaction(SIGABRT, &sigabrt_handler, NULL) != 0)
        error(EXIT_FAILURE, errno, "cannot setup handler for ABRT");

    printf("1: raise(SIGABRT)\n");
    raise(SIGABRT);

    printf("2: abort()\n");
    abort();

    printf("Still alive? Huh.\n");
    return EXIT_SUCCESS;
}
