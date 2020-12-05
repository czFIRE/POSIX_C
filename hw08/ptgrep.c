#define _GNU_SOURCE

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <error.h>
#include <pthread.h>
#include <unistd.h>

#define CANT_OPEN_FILE (void *)1
#define STRING_NOT_FOUND (void *)2

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

    bool opened;
    bool found;
};

void *thread_run(void *param)
{
    struct thread_data *searcher = param;

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    FILE *input = fopen(searcher->file_name, "r");
    if (input == NULL) {
        fprintf(stderr, "Can't open file: %s\n", searcher->file_name);
        return CANT_OPEN_FILE;
    }
    searcher->opened = true;

    char *(*string_find)(const char *, const char *);
    if (searcher->shared->insensitive) {
        string_find = strcasestr;
    } else {
        string_find = strstr;
    }

    size_t line_num = 0, string_length = strlen(searcher->shared->string);
    while ((read = getline(&line, &len, input)) != EOF) {
        char *location = string_find(line, searcher->shared->string);

        if (location != 0) {
            searcher->found = true;
            printf("%s: ", searcher->file_name);
            if (searcher->shared->line_number) {
                printf("%ld: ", line_num);
            }

            size_t offset = location - line;
            char tmp = line[offset];
            line[offset] = '\0';
            printf("%s", line);
            line[offset] = tmp;

            char * line_copy = location;

            tmp = line_copy[string_length];
            line_copy[string_length] = '\0';
            printf("\x1b[32m%s\x1b[0m", line_copy);
            line_copy[string_length] = tmp;

            //line_copy++;

            printf("%s", line_copy + string_length);

            /*while ((location = string_find(line, searcher->shared->string)) != NULL) {
                offset = location - line_copy;
                tmp = line_copy[offset];
                line[offset] = '\0';
                printf("%s", line);
                line[offset] = tmp;

                char * line_copy = location + 1;
            }*/
            
        }

        line_num++;
    }

    fclose(input);
    free(line);

    if (!searcher->found) {
        return STRING_NOT_FOUND;
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
        searcher->opened = false;
        searcher->found = false;

        if ((perrno = pthread_create(&searcher->thread, NULL, &thread_run,
                                     searcher)) != 0)
            error(EXIT_FAILURE, perrno, "pthread_create");
    }

    bool opened = true, found = false;
    for (int i = 0; i < thread_num; i++) {
        void *retval = (void *)1;
        if ((perrno = pthread_join(searchers[i].thread, &retval)) != 0)
            error(0, perrno, "pthread_join");
        opened = opened && searchers[i].opened;
        found = found || searchers[i].found;
    }

    free(searchers);
    if (!opened || !found) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
