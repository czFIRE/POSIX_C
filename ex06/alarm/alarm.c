#define _GNU_SOURCE
#define _POSIX_C_SOURCE 200809L

#include <errno.h>
#include <error.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    if (argc < 3) {
        fprintf(stderr, "Not enough args");
        return EXIT_FAILURE;
    }
    // should check whether this is a number or not
    long seconds = strtol(argv[1], NULL, 10);

    alarm(seconds);
    // Check whether this has sucessfully started
    execv(argv[2], argv + 2);

    return EXIT_SUCCESS;
}