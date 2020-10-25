#define _GNU_SOURCE

#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "no args supplied");
        return EXIT_FAILURE;
    }

    struct guess_game resources = {0};
    resources.length = argc - 1;

    resources.functions = calloc(argc - 1, sizeof(struct strategy_impl *));
    if (!resources.functions) {
        fprintf(stderr, "Functions malloc failed!");
        return EXIT_FAILURE;
    }

    resources.handles = calloc(argc - 1, sizeof(void *));
    if (!resources.handles) {
        fprintf(stderr, "Handles malloc failed!");
        cleanup(&resources);
        return EXIT_FAILURE;
    }

    if (init_libraries(argc, argv, &resources) != SUCCESS) {
        return EXIT_FAILURE;
    }

    resources.strategies = calloc(argc - 1, sizeof(void *));
    if (!resources.strategies) {
        fprintf(stderr, "Strategies malloc failed!");
        cleanup(&resources);
        return EXIT_FAILURE;
    }

    // hehe, you should parse args
    // like for real, half point is a half point
    int min = 0, max = 100;

    for (int i = 0; i < argc - 1; i++) {
        resources.strategies[i] = resources.functions[i]->init(min, max);
        if (resources.strategies[i] == NULL) {
            fprintf(stderr, "Malloc in library init failed!");
            cleanup(&resources);
            return EXIT_FAILURE;
        }
    }

    srand(time(NULL));
    size_t game_length = gameloop(min, max, &resources);
    printf("Guessing took %lu tries in total.\n", game_length);

    cleanup(&resources);

    return EXIT_SUCCESS;
}
