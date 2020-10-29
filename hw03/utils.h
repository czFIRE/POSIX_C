#ifndef UTILS_H
#define UTILS_H

#define _GNU_SOURCE

#include <stddef.h>

#define CANNARY_WORD "ZDE" // int value of 4195828
#define STATUS_INFO                                                            \
    "current: %lu\ttotal: %lu\tfreed: %lu\tallocations: %lu\tdeallocations: "  \
    "%lu\terrors: %lu"
#define NICER_STATUS_INFO                                                      \
"Final statistics:\n\
Current:\t\t%lu\n\
Total used:\t\t%lu\n\
Total freed:\t\t%lu\n\
Allocations:\t\t%lu\n\
Deallocations:\t\t%lu\n\
Errors:\t\t\t%lu\n\
Declined allocs:\t%lu"

struct memory_statistics {
    size_t total_used;
    size_t total_freed; // current in use will be the difference
    size_t allocation_num;
    size_t dealllocation_num;
    size_t errors;
    size_t declined_allocations;
};

struct service_info {
    size_t block_size;
    char *check; // magic constant
};

struct real_functions {
    void *(*real_malloc)(size_t);
    void *(*real_calloc)(size_t, size_t);
    void *(*real_realloc)(void *, size_t);
    void (*real_free)(void *);
};

struct enviroment_settings {
    size_t memgrind_mem_max;
    size_t memgrind_alloc_max;
    size_t memgrind_alloc_count;
};

void read_real_functions(struct real_functions *functions);
void initialise_service_block(struct service_info *info, size_t bytes);
int check_service_validity(struct service_info *info);
void print_statistics(struct memory_statistics *statistics);
void initialise_from_env(struct enviroment_settings *settings);
size_t get_value(char *env_var_name);

void print_final(struct memory_statistics statistics);

#endif // UTILS_H