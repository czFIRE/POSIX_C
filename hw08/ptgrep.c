#define _GNU_SOURCE

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <error.h>
#include <pthread.h>
#include <unistd.h>

#define UNUSED(x) ((void)(x))

void help(const char *argv0)
{
    printf("usage: %s [OPTIONS] STRING [FILES...]\n"
           "\n"
           "OPTIONS AND ARGUMENTS\n"
           "  -h\t\tshow this message and exit\n"
           "  -n\t\twrite out line number when the matching string has been "
           "found\n"
           "\n"
           "  STRING\tString that we are searching for in files.\n"
           "  FILES \tFiles that are meant to be searched.\n",
           argv0);
}

static const char optstring[] = "nhi";

struct shared_thread_data {
    char *string;
    bool line_number;
    bool insensitive;
};

struct thread_data {
    pthread_t thread;
    char *file_name;
    struct shared_thread_data *shared;
};

void *thread_run(void *param)
{
    struct thread_data *searcher = param;

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    size_t line_num = 0;

    // printf("opening file %s\n", searcher->file_name);
    FILE *input = fopen(searcher->file_name, "r");
    if (input == NULL) {
        fprintf(stderr, "Can't open file: %s\n", searcher->file_name);
        return (void *)1;
    }

    size_t found = 0;
    while ((read = getline(&line, &len, input)) != EOF) {
        char *location;
        if (searcher->shared->insensitive) {
            location = strcasestr(line, searcher->shared->string);
        } else {
            location = strstr(line, searcher->shared->string);
        }

        if (location != 0) {
            printf("%s: ", searcher->file_name);
            if (searcher->shared->line_number) {
                printf("%ld: ", line_num);
            }
            printf("%s", line);
        }

        line_num++;
    }

    fflush(stdout);
    fclose(input);
    free(line);

    if (found == 0) {
        return (void *)1;
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    int opt;
    struct shared_thread_data shared = {0};

    while ((opt = getopt(argc, argv, optstring)) != -1) {
        switch (opt) {
        case 'h':
            help(argv[0]);
            return EXIT_SUCCESS;

        case 'n':
            shared.line_number = true;
            break;

        case 'i':
            shared.insensitive = true;
            break;

        default:
            help(argv[0]);
            return EXIT_FAILURE;
        }
    }

    int thread_num = argc - 1 - optind;

    if (thread_num < 0) {
        fprintf(stderr, "No string specified\n");
    }

    if (thread_num < 1) {
        fprintf(stderr, "No files specified\n");
        return EXIT_FAILURE;
    }

    shared.string = argv[optind];

    struct thread_data *searchers = malloc(sizeof(*searchers) * thread_num);
    if (searchers == NULL)
        error(EXIT_FAILURE, errno, "malloc");

    int perrno;
    for (int i = 0; i < thread_num; i++) {
        struct thread_data *searcher = &searchers[i];
        searcher->file_name = argv[optind + i + 1];
        searcher->shared = &shared;

        if ((perrno = pthread_create(&searcher->thread, NULL, &thread_run,
                                     searcher)) != 0)
            error(EXIT_FAILURE, perrno, "pthread_create");
    }

    printf("Threads created\n");

    int result = 0;
    for (int i = 0; i < thread_num; i++) {
        void *retval = (void *)1;
        if ((perrno = pthread_join(searchers[i].thread, &retval)) != 0)
            error(0, perrno, "pthread_join");
        result += (int)retval;
    }
    printf("\nThreads joined\n");

    free(searchers);
    if (result == thread_num) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
