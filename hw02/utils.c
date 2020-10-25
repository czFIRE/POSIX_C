#include "utils.h"

#include <dlfcn.h>
#include <stdio.h>

int init_libraries(size_t argc, char *argv[], struct guess_game *resources)
{
    char string_mem[BUFFER_LENGTH] = "\0";

    for (size_t i = 1; i < argc; i++) {
        // check for really large input strings, may overflow here
        int error_code = sprintf(string_mem, "lib%s.so", argv[i]);

        if (error_code < 0 || error_code > BUFFER_LENGTH) {
            fprintf(stderr, "Such strategy doesn't exist: %s", argv[i]);
            cleanup(resources);
            return FAILURE;
        }

        resources->handles[i - 1] = dlopen(string_mem, RTLD_LAZY);
        void *handle = resources->handles[i - 1];

        if (!handle) {
            fprintf(stderr, "%s\n", dlerror());
            cleanup(resources);
            return FAILURE;
        }

        // should clear errors form dl
        dlerror();

        struct strategy_impl *strategy = malloc(sizeof(struct strategy_impl));

        if (!strategy) {
            fprintf(stderr, "Malloc failed");
            cleanup(resources);
            return FAILURE;
        }

        *(void **)(&strategy->init) = dlsym(handle, "init");
        *(void **)(&strategy->destroy) = dlsym(handle, "destroy");
        *(void **)(&strategy->guess) = dlsym(handle, "guess");
        *(void **)(&strategy->notify) = dlsym(handle, "notify");

        char *error;
        if ((error = dlerror()) != NULL) {
            fprintf(stderr, "%s\n", error);
            cleanup(resources);
            return FAILURE;
        }

        resources->functions[i - 1] = strategy;
    }

    return SUCCESS;
}

void cleanup(struct guess_game *resources)
{
    if (resources->functions) {
        if (resources->strategies) {
            for (size_t i = 0; i < resources->length; i++) {
                resources->functions[i]->destroy(resources->strategies[i]);
            }
            free(resources->strategies);
        }

        for (size_t i = 0; i < resources->length; i++) {
            free(resources->functions[i]);
        }
        free(resources->functions);
    }

    if (resources->handles) {
        for (size_t i = 0; i < resources->length; i++) {
            if (!resources->handles[i]) {
                break;
            }
            dlclose(resources->handles[i]);
        }
        free(resources->handles);
    }
}

int gameloop(int min, int max, struct guess_game *resources)
{
    int number_to_guess = rand() % (max - min) + min;
    printf("Number to guess is: %d\n\n", number_to_guess);

    int current_guess = 0, rotating_counter = 0;

    do {
        int index = rotating_counter % resources->length;

        current_guess =
            resources->functions[index]->guess(resources->strategies[index]);

        printf("Current guess is: %d\n", current_guess);

        if (current_guess != number_to_guess) {
            resources->functions[index]->notify(
                resources->strategies[index],
                current_guess < number_to_guess ? 1 : -1);
        } else {
            printf("\nCorrect! ");
        }

        rotating_counter++;
    } while ((current_guess != number_to_guess) && rotating_counter < 100);

    if (rotating_counter >= 100) {
        printf("\nFailed! ");
    }

    return rotating_counter;
}