#define _GNU_SOURCE

#include "dynarray.h"

#include <errno.h>
#include <error.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>

const char *optstring = "f:n";

int main(int argc, char *argv[])
{
    // int commands = 0;
    bool delete_new_line = true;

    FILE *read_from = stdin;

    int opt;
    while ((opt = getopt(argc, argv, optstring)) != -1) {
        switch (opt) {
        // maybe add some case for help?
        case 'f':
            read_from = fopen(optarg, "r");
            break;

        case 'n':
            delete_new_line = false;
            break;

        default:
            return EXIT_FAILURE;
        }
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    pid_t pid;

    while ((read = getline(&line, &len, read_from)) != EOF) {
        pid = fork();

        if (pid == -1) {
            error(EXIT_FAILURE, errno, "fork");
        }

        if (pid > 0) {
            add(pid);
            continue;
            // else after continue I know, but this is more readable for me
        } else {
            if (delete_new_line) {
                char *loc = strchr(line, '\n');
                *loc = 0;
            }

            execv(argv[optind], (argv + optind + 1));
            return EXIT_SUCCESS;
        }
    }

    if (pid > 0) {
        size_t list_len = get_length();
        ARRAY_TYPE *arr = get_array();

        for (size_t i = 0; i < list_len; i++) {
            waitpid(arr[i], NULL, 0);
            printf("killed child %d\n", arr[i]);
        }
    }

    free(line);
    free_memory();

    return EXIT_SUCCESS;
}