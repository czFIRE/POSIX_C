#define _GNU_SOURCE

#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[])
{
    // srand(time(NULL));

    if (argc < 2) {
        fprintf(stderr, "no args supplied");
        return EXIT_FAILURE;
    }

    struct strategy_impl **functions =
        calloc(argc - 1, sizeof(struct strategy_impl *));
    if (!functions) {
        fprintf(stderr, "Functions malloc failed!");
        return EXIT_FAILURE;
    }

    void **handles = calloc(argc - 1, sizeof(void *));
    if (!handles) {
        free(functions);
        fprintf(stderr, "Handles malloc failed!");
        return EXIT_FAILURE;
    }

    if (init_libraries(functions, handles, argc, argv) == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }

    void **strategies = calloc(argc - 1, sizeof(void *));
    if (!strategies) {
        cleanup(functions, handles, NULL, argc - 1);
        fprintf(stderr, "Strategies malloc failed!");
        return EXIT_FAILURE;
    }

    // hehe, you should parse args
    // like for real, half point is half point
    int min = 0, max = 100;

    for (int i = 0; i < argc - 1; i++) {
        strategies[i] = functions[i]->init(min, max);
    }

    srand(difftime(time(NULL), clock()));

    size_t game_length = gameloop(min, max, argc - 1, functions, strategies);

    printf("Game took %lu tries combined.\n", game_length);

    cleanup(functions, handles, strategies, argc - 1);

    return EXIT_SUCCESS;
}
