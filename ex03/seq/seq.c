#define _GNU_SOURCE
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <getopt.h>
#include <unistd.h>

#define BUFFER_LENGTH 80

void version() { puts("Version 6.6.6, Hail Stan!"); }

void help()
{
    puts("Print numbers from FIRST to LAST, in steps of INCREMENT.\n"
         "\n"
         "Mandatory arguments to long options are mandatory for short options "
         "too.\n"
         "\n"
         "-f, --format=FORMAT\n"
         "      use printf style floating-point FORMAT\n"
         "\n"
         "-s, --separator=STRING\n"
         "      use STRING to separate numbers (default: \\n)\n"
         "\n"
         "-w, --equal-width\n"
         "      equalize width by padding with leading zeroes\n"
         "\n"
         "--help\n"
         "      display this help and exit\n"
         "\n"
         "--version\n"
         "       output version information and exit\n");
}

static const char optstring[] = "vhws:f:";
struct option longopts[] = {
    {.val = 'v', .name = "version"},
    {.val = 'h', .name = "help"},
    {.val = 'w', .name = "equal-width"},
    {.val = 's', .name = "separator", .has_arg = required_argument},
    {.val = 'f', .name = "format", .has_arg = required_argument},
    {0},
};

struct options {
    bool equal_width;
    char *separator;
    char *format;
};

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "No args bro\n");
        return (EXIT_FAILURE);
    }
    // initialize options
    struct options options = {false, "\n", "%d"};

    int opt;

    while ((opt = getopt_long(argc, argv, optstring, longopts, NULL)) != -1) {
        switch (opt) {
        case 'h':
            help();
            return EXIT_SUCCESS;

        case 'v':
            version();
            return EXIT_SUCCESS;

        case 'w':
            options.equal_width = true;
            break;
        case 's':
            options.separator = optarg;
            break;
        case 'f':
            options.format = optarg;
            break;

        default:
            help();
            return EXIT_FAILURE;
        }
    }

    int start = 1, incr = 1, last;

    switch (argc - optind - 1) {
        // fallthrough intended
    case 2:
        // should add check for strtol here for all options
        incr = strtol(argv[optind + 1], NULL, 10);
    case 1:
        start = strtol(argv[optind], NULL, 10);
    case 0:
        last = strtol(argv[argc - 1], NULL, 10);
        break;
    default:
        fprintf(stderr, "Too many numbers specified!\n");
        help();
        return EXIT_FAILURE;
    }

    while (start <= last) {
        printf(options.format, start);
        start += incr;
        if (start <= last) {
            printf("%s", options.separator);
        }
    }

    putchar('\n');

    return EXIT_SUCCESS;
}