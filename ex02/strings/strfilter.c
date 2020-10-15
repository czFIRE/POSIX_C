#define _GNU_SOURCE

#include "libfilter.h"
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_LENGTH 80

void create_lib_name(char memory[], char *name)
{
    memset(memory, '\0', BUFFER_LENGTH);

    strcpy(memory, "lib");
    strcat(memory, name);
    strcat(memory, ".so");
}

int main(int argc, char *argv[])
{
    char string_mem[BUFFER_LENGTH] = "\0";

    if (argc < 2) {
        fprintf(stderr, "no args supplied");
        return EXIT_FAILURE;
    }

    void (*functions[argc - 1])(char *);
    void *handles[argc - 1];

    for (size_t i = 1; i < argc; i++) {
        void (*filter)(char *);

        create_lib_name(string_mem, argv[i]);

        handles[i - 1] = dlopen(string_mem, RTLD_LAZY);
        void *handle = handles[i - 1];

        if (!handle) {
            // should close all remaining handles but am lazy
            fprintf(stderr, "%s\n", dlerror());
            return EXIT_FAILURE;
        }

        // should clear errors form dl
        dlerror();

        *(void **)(&filter) = dlsym(handle, "filter");

        char *error;
        if ((error = dlerror()) != NULL) {
            fprintf(stderr, "%s\n", error);
            return EXIT_FAILURE;
        }

        functions[i - 1] = filter;
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    int rotating_counter = 0;
    while ((read = getline(&line, &len, stdin)) != EOF) {
        (*(functions[rotating_counter % (argc - 1)]))(line);
        printf("%s", line);
        rotating_counter++;
    }

    free(line);

    for (size_t i = 0; i < argc - 1; i++) {
        dlclose(handles[i]);
    }

    return EXIT_SUCCESS;
}