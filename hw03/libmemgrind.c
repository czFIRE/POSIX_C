#include "libmemgrind.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>

static struct real_functions functions = {0};
static struct memory_statistics statistics = {0};
const int SERVICE_LENGTH = sizeof(struct service_info);
static struct enviroment_settings allocation_setting;

__attribute__((constructor)) static void library_init(void)
{
    // openlog + maximum messages -> setlogmask()
    // change this to just write to

    // parse enviroment variables

    // openlog("", LOG_PERROR, LOG_USER);
    read_enviroment_settings(&allocation_setting);
    read_real_functions(&functions);
    syslog(LOG_INFO, "library loaded succefully");
}

__attribute__((destructor)) static void library_destroy(void)
{
    fprintf(stderr, "cleanup\n");

    syslog(LOG_INFO, NICER_STATUS_INFO,
           statistics.total_used - statistics.total_freed,
           statistics.total_used, statistics.total_freed,
           statistics.allocation_num, statistics.dealllocation_num);

    if (statistics.allocation_num > statistics.dealllocation_num) {
        syslog(LOG_WARNING, "Possible memory leak of size %lu in %lu blocks",
               statistics.total_used - statistics.total_freed,
               statistics.allocation_num - statistics.dealllocation_num);
    }

    closelog();
}

static volatile int libc_guard = 0;

// my malloc
void *malloc(size_t size)
{
    // fprintf(stderr, "calling malloc\n");
    if (libc_guard) {
        return functions.real_malloc(size);
    }
    libc_guard = 1;

    syslog(LOG_INFO, "malloc(%lu)", size);
    statistics.allocation_num++;
    statistics.total_used = statistics.total_used + size;

    void *memory = functions.real_malloc(size + SERVICE_LENGTH);
    if (!memory) {
        statistics.errors++;
        syslog(LOG_ERR, "malloc failed");
        return NULL;
    }
    initialise_service_block(memory, size);

    print_statistics(&statistics);
    libc_guard = 0;
    return memory + SERVICE_LENGTH;
}

// my calloc
void *calloc(size_t num, size_t size)
{
    // fprintf(stderr, "calling calloc\n");
    if (libc_guard) {
        return functions.real_calloc(num, size);
    }
    libc_guard = 1;

    syslog(LOG_INFO, "calloc(%lu, %lu)", num, size);
    statistics.allocation_num++;
    statistics.total_used = statistics.total_used + (num * size);

    void *memory = functions.real_calloc(num * size + SERVICE_LENGTH, 1);

    if (!memory) {
        statistics.errors++;
        syslog(LOG_ERR, "calloc failed");
        return NULL;
    }
    initialise_service_block(memory, num * size);

    print_statistics(&statistics);
    libc_guard = 0;
    return memory + SERVICE_LENGTH;
}

// my realloc
void *realloc(void *ptr, size_t size)
{
    // what if *ptr is NULL -> do smth about it
    // fprintf(stderr, "calling realloc\n");

    if (libc_guard) {
        return functions.real_realloc(ptr, size);
    }
    libc_guard = 1;

    syslog(LOG_INFO, "realloc(%p, %lu)", ptr, size);
    if (ptr == NULL) {
        return malloc(size);
    }

    ptr = ptr - SERVICE_LENGTH;

    if (check_service_validity(ptr) != 0) {
        syslog(LOG_ERR, "service struct missing");
        statistics.errors++;
        return NULL;
    }

    size_t block_size = ((struct service_info *)ptr)->block_size;

    struct service_info *memory =
        functions.real_realloc(ptr, size + SERVICE_LENGTH);
    if (!memory) {
        statistics.errors++;
        syslog(LOG_ERR, "realloc failed");
        return NULL;
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

    memory->block_size = size;

    print_statistics(&statistics);
    libc_guard = 0;
    return (void *)memory + SERVICE_LENGTH;
}

// my free
void free(void *ptr)
{
    // fprintf(stderr, "calling free\n");

    if (libc_guard) {
        return functions.real_free(ptr);
    }
    libc_guard = 1;

    syslog(LOG_INFO, "free(%p)", ptr);
    if (ptr == NULL) {
        return;
    }

    ptr = ptr - SERVICE_LENGTH;
    if (check_service_validity(ptr) != 0) {
        syslog(LOG_ERR, "service struct missing");
        statistics.errors++;
        return;
    }

    statistics.dealllocation_num++;
    statistics.total_freed += ((struct service_info *)ptr)->block_size;

    print_statistics(&statistics);
    libc_guard = 0;
    functions.real_free(ptr);
}
