#ifndef UTILS_H
#define UTILS_H

#define _GNU_SOURCE

#include <stddef.h>

#define CANNARY_WORD "ZDE" // int value of 4195828

struct memory_statistics {
    size_t total_used;
    size_t total_freed; // current in use will be the difference
    size_t allocation_num;
    size_t dealllocation_num;
    size_t errors;
};

struct service_info {
    size_t block_size;
    char* check; // magic constant
};

struct real_functions {
    void *(*real_malloc)(size_t);
    void *(*real_calloc)(size_t, size_t);
    void *(*real_realloc)(void *, size_t);
    void (*real_free)(void *);
};

void read_real_functions(struct real_functions *functions);
void initialise_service_block(struct service_info * info, size_t bytes);
int check_service_validity(struct service_info * info);

#endif // UTILS_H