#include "libmemgrind.h"
#include "utils.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>

struct real_functions functions = {0};
struct memory_statistics statistics = {0};
struct enviroment_settings allocation_setting = {0};

const int SERVICE_LENGTH = sizeof(struct service_info);
static volatile int libc_guard = 1;

bool can_alloc(size_t size);
void *user_to_lib(char *ptr);
void *lib_to_user(char *ptr);

#define RETURN_FROM_GUARDED(...)                                               \
    do {                                                                       \
        syslog(LOG_DEBUG, "Leaving guarded space");                            \
        libc_guard = 0;                                                        \
        return __VA_ARGS__;                                                    \
    } while (0)

__attribute__((constructor)) static void library_init(void)
{
    read_real_functions(&functions);
    initialise_from_env(&allocation_setting);
    syslog(LOG_INFO, "library loaded succefully");
    libc_guard = 0;
}

__attribute__((destructor)) static void library_destroy(void)
{
    // maybe remove this if last allocs / frees from syslog are important
    libc_guard = 1;
    print_final(statistics);
    syslog(LOG_INFO, "Memgrind says hello.");
    closelog();
}

void *malloc(size_t size)
{
    if (libc_guard) {
        return functions.real_malloc(size);
    }

    libc_guard = 1;
    syslog(LOG_DEBUG, "Entering guarded space");

    syslog(LOG_INFO, "malloc(%lu)", size);

    if (!can_alloc(size)) {
        RETURN_FROM_GUARDED(NULL);
    }

    statistics.allocation_num++;
    statistics.total_used = statistics.total_used + size;

    void *memory = functions.real_malloc(size + SERVICE_LENGTH);
    if (!memory) {
        statistics.errors++;
        syslog(LOG_ERR, "malloc failed");
        RETURN_FROM_GUARDED(NULL);
    }
    initialise_service_block(memory, size);

    print_statistics(&statistics);
    RETURN_FROM_GUARDED(lib_to_user(memory));
}

void *calloc(size_t num, size_t size)
{
    if (libc_guard) {
        return functions.real_calloc(num, size);
    }
    libc_guard = 1;
    syslog(LOG_DEBUG, "Entering guarded space");

    syslog(LOG_INFO, "calloc(%lu, %lu)", num, size);

    if (!can_alloc(size)) {
        RETURN_FROM_GUARDED(NULL);
    }

    statistics.allocation_num++;
    statistics.total_used = statistics.total_used + (num * size);

    void *memory = functions.real_calloc(num * size + SERVICE_LENGTH, 1);

    if (!memory) {
        statistics.errors++;
        syslog(LOG_ERR, "calloc failed");
        RETURN_FROM_GUARDED(NULL);
    }
    initialise_service_block(memory, num * size);

    print_statistics(&statistics);
    RETURN_FROM_GUARDED(lib_to_user(memory));
}

void *realloc(void *ptr, size_t size)
{
    if (ptr == NULL) {
        return malloc(size);
    }

    if (libc_guard) {
        return functions.real_realloc(ptr, size);
    }
    libc_guard = 1;
    syslog(LOG_DEBUG, "Entering guarded space");

    syslog(LOG_INFO, "realloc(%p, %lu)", ptr, size);

    if (!can_alloc(size)) {
        RETURN_FROM_GUARDED(NULL);
    }

    ptr = user_to_lib(ptr);
    if (check_service_validity(ptr) != 0) {
        syslog(LOG_ERR, "service struct missing");
        statistics.errors++;
        RETURN_FROM_GUARDED(NULL);
    }

    size_t block_size = ((struct service_info *)ptr)->block_size;

    void *memory = functions.real_realloc(ptr, size + SERVICE_LENGTH);
    if (!memory) {
        statistics.errors++;
        syslog(LOG_ERR, "realloc failed");
        RETURN_FROM_GUARDED(NULL);
    }

    if (ptr == memory) { // was able to extend / contract space used
        if (size >= block_size) {
            statistics.total_used += (size - block_size);
        } else {
            statistics.total_freed += (block_size - size);
        }
    } else {
        statistics.allocation_num++;
        statistics.dealllocation_num++;
        statistics.total_used += size;
        statistics.total_freed += block_size;
    }

    ((struct service_info *)memory)->block_size = size;

    print_statistics(&statistics);
    RETURN_FROM_GUARDED(lib_to_user(memory));
}

void free(void *ptr)
{
    if (libc_guard) {
        functions.real_free(ptr);
        return;
    }
    libc_guard = 1;
    syslog(LOG_DEBUG, "Entering guarded space");

    syslog(LOG_INFO, "free(%p)", ptr);
    if (ptr == NULL) {
        RETURN_FROM_GUARDED();
    }

    ptr = user_to_lib(ptr);
    if (check_service_validity(ptr) != 0) {
        syslog(LOG_ERR, "service struct missing");
        statistics.errors++;
        RETURN_FROM_GUARDED();
    }

    statistics.dealllocation_num++;
    statistics.total_freed += ((struct service_info *)ptr)->block_size;

    print_statistics(&statistics);
    functions.real_free(ptr);
    RETURN_FROM_GUARDED();
}

bool can_alloc(size_t size)
{
    if (allocation_setting.memgrind_alloc_max < size) {
        syslog(LOG_ERR, "Declined allocation of block of size %lu (max_size)",
               size);
        statistics.declined_allocations++;
        return false;
    }

    if (allocation_setting.memgrind_mem_max < statistics.total_used + size) {
        syslog(LOG_ERR, "Declined allocation of block of size %lu (total_size)",
               size);
        statistics.declined_allocations++;
        return false;
    }

    if (allocation_setting.memgrind_alloc_count <
        statistics.allocation_num + 1) {
        syslog(LOG_ERR,
               "Declined allocation of block of size %lu (alloc_count)", size);
        statistics.declined_allocations++;
        return false;
    }

    return true;
}

void *user_to_lib(char *ptr)
{
    // fprintf(stderr, "\t\t\treceived from user:%p\t\n", ptr);
    return (ptr - SERVICE_LENGTH);
}

void *lib_to_user(char *ptr)
{
    // fprintf(stderr, "\t\t\treturned to user:%p\t\n", ptr);
    return (ptr + SERVICE_LENGTH);
}