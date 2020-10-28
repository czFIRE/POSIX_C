#include "utils.h"

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

void read_real_functions(struct real_functions *functions)
{
    functions->real_calloc = dlsym(RTLD_NEXT, "calloc");
    functions->real_free = dlsym(RTLD_NEXT, "free");
    functions->real_malloc = dlsym(RTLD_NEXT, "malloc");
    functions->real_realloc = dlsym(RTLD_NEXT, "realloc");
    if (!functions->real_calloc || !functions->real_free ||
        !functions->real_malloc || !functions->real_realloc) {
        fprintf(stderr, "Error in dlsym: %s\n", dlerror());
        exit(EXIT_FAILURE);
    }
}

void initialise_service_block(struct service_info *info, size_t bytes)
{
    info->block_size = bytes;
    info->check = CANNARY_WORD;
}

int check_service_validity(struct service_info *info)
{
    return strcmp(info->check, CANNARY_WORD);
}

void print_statistics(struct memory_statistics* statistics)
{
    syslog(LOG_DEBUG, STATUS_INFO,
           statistics->total_used - statistics->total_freed,
           statistics->total_used, statistics->total_freed,
           statistics->allocation_num, statistics->dealllocation_num);
}