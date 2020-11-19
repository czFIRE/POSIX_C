#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <error.h>
#include <getopt.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>

const char *optstring = "abs";

#define UNUSED(VAR) (void)(VAR)

void sigalrm_ignore(int sig)
{
    UNUSED(sig);
    fprintf(stderr, "SIGALRM caught\n");
}

void part_b(void);
void part_s(void);

int main(int argc, char *argv[])
{
    unsigned int seconds;
    int opt;

    while ((opt = getopt(argc, argv, optstring)) != -1) {
        switch (opt) {
        // maybe add some case for help?
        case 'a':
            seconds = alarm(0);
            printf("Time remaining: %d\n", seconds);
            sleep(10 * seconds);
            break;

        case 'b':
            part_b();
            break;

        case 's':
            part_s();
            break;

        default:
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}

void part_b(void)
{
    sigset_t sigint_set;
    sigemptyset(&sigint_set);
    sigaddset(&sigint_set, SIGALRM);
    // this can fail, check it
    sigprocmask(SIG_BLOCK, &sigint_set, NULL);

    //čekám na signál a ten z nebe nepřichází a tak čekám dál :D
    pause();
}

void part_s(void)
{
    struct sigaction sigalrm_handler = {
        .sa_handler = &sigalrm_ignore,
        .sa_flags = SA_RESTART, //tohle mi pak obnoví to, kde jsem byl
    };

    if (sigaction(SIGALRM, &sigalrm_handler, NULL) != 0)
        error(EXIT_FAILURE, errno, "cannot setup handler for ABRT");

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    while (1) {
        read = getline(&line, &len, stdin);
        if (read > 0) {
            printf("%s", line);
        }

        if (errno != 0) {
            break;
        }
    }
}