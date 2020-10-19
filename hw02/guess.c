#define _GNU_SOURCE

#include "strategy.h"
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BUFFER_LENGTH 80

struct strategy_impl {
    void *(*init)(long, long);
    void (*destroy)(void *);
    long (*guess)(void *);
    void (*notify)(void *, int);
};

// could be replaced with asprintf
void create_lib_name(char memory[], char *name)
{
    memset(memory, '\0', BUFFER_LENGTH);

    strcpy(memory, "lib");
    strcat(memory, name);
    strcat(memory, ".so");
}

int main(int argc, char *argv[])
{
    //srand(time(NULL));

    char string_mem[BUFFER_LENGTH] = "\0";

    if (argc < 2) {
        fprintf(stderr, "no args supplied");
        return EXIT_FAILURE;
    }

    // replace with fucking proper malloc mate
    struct strategy_impl *functions[argc - 1];
    void *handles[argc - 1];

    for (int i = 1; i < argc; i++) {
        create_lib_name(string_mem, argv[i]);

        handles[i - 1] = dlopen(string_mem, RTLD_LAZY);
        void *handle = handles[i - 1];

        if (!handle) {
            // should close all remaining handles and functions
            fprintf(stderr, "%s\n", dlerror());
            return EXIT_FAILURE;
        }

        // should clear errors form dl
        dlerror();

        struct strategy_impl *strategy = malloc(sizeof(struct strategy_impl));

        if (!strategy) {
            fprintf(stderr, "Malloc failed");
            return EXIT_FAILURE;
        }

        *(void **)(&strategy->init) = dlsym(handle, "init");
        *(void **)(&strategy->destroy) = dlsym(handle, "destroy");
        *(void **)(&strategy->guess) = dlsym(handle, "guess");
        *(void **)(&strategy->notify) = dlsym(handle, "notify");

        char *error;
        if ((error = dlerror()) != NULL) {
            // should free strategy, functions and handles
            fprintf(stderr, "%s\n", error);
            return EXIT_FAILURE;
        }

        functions[i - 1] = strategy;
    }

    srand(difftime(time(NULL), clock()));

    int min = 0, max = 100;
    int number_to_guess = rand() % (max - min) + min;
    printf("Number to guess is: %d\n\n", number_to_guess);

    // replace with proper malloc
    void *strategies[argc - 1];

    for (int i = 0; i < argc - 1; i++) {
        strategies[i] = functions[i]->init(min, max);
    }

    int current_guess = 0;
    int rotating_counter = 0;

    do {
        int index = rotating_counter % (argc - 1);

        current_guess = functions[index]->guess(strategies[index]);

        printf("Current guess is: %d\n", current_guess);

        if (current_guess != number_to_guess) {
            functions[index]->notify(strategies[index],
                                     current_guess < number_to_guess ? 1 : -1);
        }

        rotating_counter++;
    } while ((current_guess != number_to_guess) && rotating_counter < 100);

    // udělej si pole void* a získej si inicializované knihovny
    // game loop -> vezmu strategii i, nechám ji hádat, notify / win
    // max (max - min) game loopů

    for (int i = 0; i < argc - 1; i++) {
        functions[i]->destroy(strategies[i]);
    }

    // uvolni pole funkcí
    for (int i = 0; i < argc - 1; i++) {
        free(functions[i]);
    }

    for (int i = 0; i < argc - 1; i++) {
        dlclose(handles[i]);
    }

    return EXIT_SUCCESS;
}