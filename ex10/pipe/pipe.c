#include <err.h>
#include <errno.h>
#include <error.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>

void print_argv(char *argv[])
{
    while (*argv != NULL) {
        puts(*argv);
        argv++;
    }
}

// potřebuji 2 forky => nemáme se jak dozvědět, že skončili
// též potřebuji vědět to, že je mám ukončit

int child(int pipefd[2], char *argv[])
{
    // we don't need the reading handle
    close(pipefd[0]);

    // copy writing end of pipe to stdout
    if (dup2(pipefd[1], STDOUT_FILENO) == -1)
        err(EXIT_FAILURE, "dup2");

    // execute first program
    execvp(argv[0], argv);
    err(EXIT_FAILURE, "execvp");
}

int parent(int pipefd[2], char *argv[])
{
    // we don't need the writing handle
    close(pipefd[1]);

    // copy reading end of pipe to stdin
    if (dup2(pipefd[0], STDIN_FILENO) == -1)
        err(EXIT_FAILURE, "dup2");

    // signal EOF
    // close(pipefd[0]);

    // execute the second program
    execvp(argv[0], argv);
    err(EXIT_FAILURE, "execvp");

}

int main(int argc, char *argv[])
{
    size_t loc = 0;
    for (; argv[loc] != NULL; loc++) {
        if (strcmp(":", argv[loc]) == 0) {
            break;
        }
    }

    if ((int)loc == argc) {
        err(EXIT_FAILURE, "expected semicolon");
    }

    char **argv1, **argv2;

    argv1 = calloc(loc - 1 + 1, sizeof(char *));
    if (!argv1)
        error(EXIT_FAILURE, errno, "calloc");

    argv2 = calloc(argc - loc + 1, sizeof(char *));
    if (!argv2) {
        free(argv1);
        error(EXIT_FAILURE, errno, "calloc");
    }

    memcpy(argv1, argv + 1, (loc - 1) * sizeof(char *));
    memcpy(argv2, argv + loc + 1, (argc - loc) * sizeof(char *));

    int pfd[2];
    pipe(pfd);

    pid_t pid = fork();

    if (pid == -1)
        err(EXIT_FAILURE, "fork");

    if (pid == 0)
        child(pfd, argv2);
    else
        parent(pfd, argv1);

    return EXIT_SUCCESS;
}
