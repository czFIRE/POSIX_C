#include "utils.h"

#include <dlfcn.h>
#include <stdio.h>

int init_libraries(struct strategy_impl *functions[], void *handles[],
                   size_t argc, char *argv[])
{
    char string_mem[BUFFER_LENGTH] = "\0";

    for (size_t i = 1; i < argc; i++) {
        // check for really large input strings, may overflow here
        int error_code = sprintf(string_mem, "lib%s.so", argv[i]);

        if (error_code < 0 || error_code > 80) {
            cleanup(functions, handles, NULL, argc - 1);
            fprintf(stderr, "Such strategy doesn't exist: %s", argv[i]);
            return EXIT_FAILURE;
        }

        handles[i - 1] = dlopen(string_mem, RTLD_LAZY);
        void *handle = handles[i - 1];

        if (!handle) {
            cleanup(functions, handles, NULL, argc - 1);
            fprintf(stderr, "%s\n", dlerror());
            return EXIT_FAILURE;
        }

        // should clear errors form dl
        dlerror();

        struct strategy_impl *strategy = malloc(sizeof(struct strategy_impl));

        if (!strategy) {
            cleanup(functions, handles, NULL, argc - 1);
            fprintf(stderr, "Malloc failed");
            return EXIT_FAILURE;
        }

        *(void **)(&strategy->init) = dlsym(handle, "init");
        *(void **)(&strategy->destroy) = dlsym(handle, "destroy");
        *(void **)(&strategy->guess) = dlsym(handle, "guess");
        *(void **)(&strategy->notify) = dlsym(handle, "notify");

        char *error;
        if ((error = dlerror()) != NULL) {
            cleanup(functions, handles, NULL, argc - 1);
            fprintf(stderr, "%s\n", error);
            return EXIT_FAILURE;
        }

        functions[i - 1] = strategy;
    }

    return EXIT_SUCCESS;
}

void cleanup(struct strategy_impl *functions[], void *handles[],
             void *strategies[], size_t length)
{
    if (functions) {
        if (strategies) {
            for (size_t i = 0; i < length; i++) {
                functions[i]->destroy(strategies[i]);
            }
            free(strategies);
        }

        for (size_t i = 0; i < length; i++) {
            free(functions[i]);
        }
        free(functions);
    }

    if (handles) {
        for (size_t i = 0; i < length; i++) {
            if (!handles[i]) {
                break;
            }
            dlclose(handles[i]);
        }
        free(handles);
    }
}

int gameloop(int min, int max, size_t length, struct strategy_impl *functions[],
             void *strategies[])
{
    int number_to_guess = rand() % (max - min) + min;
    printf("Number to guess is: %d\n\n", number_to_guess);

    int current_guess = 0, rotating_counter = 0;

    do {
        int index = rotating_counter % (length);

        current_guess = functions[index]->guess(strategies[index]);

        printf("Current guess is: %d\n", current_guess);

        if (current_guess != number_to_guess) {
            functions[index]->notify(strategies[index],
                                     current_guess < number_to_guess ? 1 : -1);
        }

        rotating_counter++;
    } while ((current_guess != number_to_guess) && rotating_counter < 100);

    return rotating_counter;
}