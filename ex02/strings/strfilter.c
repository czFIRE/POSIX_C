#include "libfilter.h"
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define _GNU_SOURCE

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "wrong amount of args");
        return EXIT_FAILURE;
    }

    char * string_mem = calloc(strlen(argv[1]) + strlen(".so") + strlen("lib") + 1, sizeof(char));
    if (!string_mem) {
        fprintf(stderr, "malloc failed");
        return EXIT_FAILURE;
    }

    strcpy(string_mem, "lib");
    strcat(string_mem, argv[1]);
    strcat(string_mem, ".so");

    void (*filter)(char *);
    void *handle = dlopen(string_mem, RTLD_LAZY);

    free(string_mem);

    if(!handle) {
        fprintf(stderr, "%s\n", dlerror());
        return EXIT_FAILURE;
    }

    // should clear errors form dl
    dlerror();

    * (void **) (&filter) = dlsym(handle, "filter");
    
    char *error;
    if ((error = dlerror()) != NULL) {
        fprintf(stderr, "%s\n", error);
        return EXIT_FAILURE;
    }

    // add reading from stdin using readline

    char text[] = "ahOJ\nAHoj";
    (*filter)(text);

    printf("%s\n", text);
    dlclose(handle);

    return EXIT_SUCCESS;
}