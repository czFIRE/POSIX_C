#define _GNU_SOURCE
#include "utils.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <getopt.h>
#include <unistd.h>

static const char optstring[] = "gGnruhv";
struct option longopts[] = {
    {.val = 'h', .name = "version"}, {.val = 'v', .name = "help"},
    {.val = 'g', .name = "group"},   {.val = 'G', .name = "groups"},
    {.val = 'n', .name = "name"},    {.val = 'r', .name = "real"},
    {.val = 'u', .name = "user"},    {0},
};

void print_options(struct program_options options)
{
    printf("Group:\t%d\nGroups:\t%d\nName:\t%d\nReal:\t%d\nUser:\t%d\n",
           options.group, options.groups, options.name, options.real,
           options.user);
}

int main(int argc, char *argv[])
{
    int opt;
    struct program_options options;
    memset(&options, 0, sizeof(struct program_options));

    while ((opt = getopt_long(argc, argv, optstring, longopts, NULL)) != -1) {
        switch (opt) {
        case 'h':
            help();
            return EXIT_SUCCESS;

        case 'v':
            version();
            return EXIT_SUCCESS;

        case 'g':
            options.group = true;
            break;
        case 'G':
            options.groups = true;
            break;
        case 'n':
            options.name = true;
            break;
        case 'r':
            options.real = true;
            break;
        case 'u':
            options.user = true;
            break;

        default:
            help();
            return EXIT_FAILURE;
        }
    }

    // print_options(options);

    /*get_user_info("1000");
    return EXIT_SUCCESS;*/

    if ((options.group + options.groups + options.user) > 1) {
        fprintf(stderr, "You can't set parameters -u, -g, -G in parallel\n");
        return EXIT_FAILURE;
    }

    if (((options.group + options.groups + options.user) == 0) &&
        (options.real || options.name)) {
        fprintf(stderr, "Can't use name or real ID in default mode\n");
        return EXIT_FAILURE;
    }

    if (argc == optind) {
        get_process_info(options);
    } else {
        for (int i = optind; i < argc; ++i) {
            get_user_info(argv[i], options);
        }
    }

    return EXIT_SUCCESS;
}