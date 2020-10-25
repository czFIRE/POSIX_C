#define _GNU_SOURCE

#include "libmemgrind.h"

#include <stdlib.h>
#include <dlfcn.h>
#include <stdio.h>

//static volatile libc_guard = 0;
static struct real_functions functions = {0};

__attribute__((constructor)) static void library_init(void) {
    printf("started loading");
    functions.real_calloc = dlsym(RTLD_NEXT, "calloc");
    functions.real_free = dlsym(RTLD_NEXT, "free");
    functions.real_malloc = dlsym(RTLD_NEXT, "malloc");
    functions.real_realloc = dlsym(RTLD_NEXT, "realloc");

    if (!functions.real_calloc || !functions.real_free || !functions.real_malloc || !functions.real_realloc) {
        fprintf(stderr, "Error in dlsym: %s\n", dlerror());
        exit(EXIT_FAILURE);
    }
    printf("loaded functions\n");
}

__attribute__((destructor)) static void library_destroy(void) {
    dlclose(functions.real_calloc);
    dlclose(functions.real_free);
    dlclose(functions.real_malloc);
    dlclose(functions.real_realloc);
    printf("freed functions\n");
}

//my malloc
void *malloc(size_t size) {
    /*if (libc_guard)
        return __libc_malloc(size);

    libc_guard = 1;

    //syslog

    libc_guard = 0;*/
    printf("calling malloc\n");
    return functions.real_malloc(size);
}

//my calloc
void *calloc(size_t num, size_t size) {
    printf("calling calloc\n");
    return functions.real_calloc(num, size);
}

//my realloc
void *realloc(void *ptr, size_t size) {
    printf("calling realloc\n");
    return functions.real_realloc(ptr, size);
}

//my free
void free(void *ptr) {
    printf("calling free\n");
    return functions.real_free(ptr);
}