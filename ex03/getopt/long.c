#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <getopt.h>
#include <unistd.h>

void help(const char *argv0)
{
    printf(
        "usage: %s [OPTIONS] ARGUMENTS...\n"
        "\n"
        "OPTIONS AND ARGUMENTS\n"
        "  -h,--help        show this message and exit\n"
        "  -v,--version     show program version and exit\n"
        "\n"
        "  -a,--alpha       turn on option a\n"
        "  -b,--beta        turn on option b\n"
        "  -g,--gamma ARG   set ARG as a value for option gamma\n"
    , argv0);
}

void version(void)
{
    puts("short 1.0");
}

static const char optstring[] = "hvabc:";
struct option longopts[] = {
    { .val = 'h', .name = "version" },
    { .val = 'v', .name = "help" },
    { .val = 'a', .name = "alpha" },
    { .val = 'b', .name = "beta" },
    { .val = 'c', .name = "gamma", .has_arg = required_argument },
    { 0 },
};

struct options {
    bool alpha;
    bool beta;
    char *gamma;
};

int main(int argc, char *argv[])
{
    int opt;

    // initialize options
    struct options options;
    memset(&options, 0, sizeof(struct options));

    // call getopt() until there are no options left
    while ((opt = getopt_long(argc, argv, optstring, longopts, NULL)) != -1) {
        switch (opt) {
        case 'h':
            help(argv[0]);
            return EXIT_SUCCESS;

        case 'v':
            version();
            return EXIT_SUCCESS;

        case 'a':
            options.alpha = true;
            break;
        case 'b':
            options.beta = true;
            break;
        case 'c':
            options.gamma = optarg;
            break;

        default:
            help(argv[0]);
            return EXIT_FAILURE;
        }
    }

    // print options
    printf("OPTIONS:\n");
    if (options.alpha)
        printf("\x1b[32m * \x1b[0moption alpha is set\n");
    else
        printf("\x1b[31m * \x1b[0moption alpha is not set\n");

    if (options.beta)
        printf("\x1b[32m * \x1b[0moption beta is set\n");
    else
        printf("\x1b[31m * \x1b[0moption beta is not set\n");

    if (options.gamma != NULL)
        printf("\x1b[32m * \x1b[0moption gamma is set to value \x1b[33m%s\x1b[0m\n", options.gamma);
    else
        printf("\x1b[31m * \x1b[0moption gamma is not set\n");

    // left-over arguments are in argv[optind] ... argv[argc - 1]
    printf("ARGUMENTS:\n");
    for (int i = optind; i < argc; ++i) {
        printf("argv[%u] = \x1b[33m%s\x1b[0m\n", i, argv[i]);
    }

    return EXIT_SUCCESS;
}
