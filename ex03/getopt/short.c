#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <unistd.h>

void help(const char *argv0)
{
    printf(
        "usage: %s [OPTIONS] ARGUMENTS...\n"
        "\n"
        "OPTIONS AND ARGUMENTS\n"
        "  -h       show this message and exit\n"
        "  -v       show program version and exit\n"
        "\n"
        "  -a       turn on option a\n"
        "  -b       turn on option b\n"
        "  -c ARG   set ARG as a value for option c\n"
    , argv0);
}

void version(void)
{
    puts("short 1.0");
}

static const char optstring[] = "hvabc:";

struct options {
    bool a;
    bool b;
    char *c;
};

int main(int argc, char *argv[])
{
    int opt;

    // initialize options
    struct options options;
    memset(&options, 0, sizeof(struct options));

    // call getopt() until there are no options left
    while ((opt = getopt(argc, argv, optstring)) != -1) {
        switch (opt) {
        case 'h':
            help(argv[0]);
            return EXIT_SUCCESS;

        case 'v':
            version();
            return EXIT_SUCCESS;

        case 'a':
            options.a = true;
            break;
        case 'b':
            options.b = true;
            break;
        case 'c':
            options.c = optarg;
            break;

        default:
            help(argv[0]);
            return EXIT_FAILURE;
        }
    }

    // print options
    printf("OPTIONS:\n");
    if (options.a)
        printf("\x1b[32m * \x1b[0moption a is set\n");
    else
        printf("\x1b[31m * \x1b[0moption a is not set\n");

    if (options.b)
        printf("\x1b[32m * \x1b[0moption b is set\n");
    else
        printf("\x1b[31m * \x1b[0moption b is not set\n");

    if (options.c != NULL)
        printf("\x1b[32m * \x1b[0moption c is set to value \x1b[33m%s\x1b[0m\n", options.c);
    else
        printf("\x1b[31m * \x1b[0moption c is not set\n");

    // left-over arguments are in argv[optind] ... argv[argc - 1]
    printf("ARGUMENTS:\n");
    for (int i = optind; i < argc; ++i) {
        printf("argv[%u] = \x1b[33m%s\x1b[0m\n", i, argv[i]);
    }

    return EXIT_SUCCESS;
}
