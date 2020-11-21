#define _GNU_SOURCE
#include "utils.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <getopt.h>
#include <unistd.h>

static const char optstring[] = "dvi:";

int main(int argc, char *argv[])
{
    int opt;
    struct program_options options;
    memset(&options, 0, sizeof(struct program_options));

    while ((opt = getopt(argc, argv, optstring)) != -1) {
        switch (opt) {
        case 'h':
            help();
            return EXIT_SUCCESS;

        case 'v':
            options.error = true;
            break;

        case 'd':
            options.demon = true;
            break;
        case 'i': {
            char *endp;
            int number = strtol(optarg, &endp, 10);
            if ((endp == optarg) || (*endp != '\0')) {
                fprintf(stderr, "wrong input - expected number\n");
                return EXIT_FAILURE;
            }
            options.interval = number;
            } 
            break;

        default:
            help();
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}