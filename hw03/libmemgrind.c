#include "libmemgrind.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>

static struct real_functions functions = {0};
static struct memory_statistics statistics = {0};
const int SERVICE_LENGTH = sizeof(struct service_info);

__attribute__((constructor)) static void library_init(void)
{
    fprintf(stderr, "started loading\n");
    // openlog + maximum messages -> setlogmask()
    read_real_functions(&functions);
}

__attribute__((destructor)) static void library_destroy(void)
{
    fprintf(stderr, "cleanup\n");
    fprintf(stderr, "\nCurrent: %lu\n\
Total used: %lu\n\
Total freed: %lu\n\
Allocations: %lu\n\
Deallocations: %lu\n\
",
            statistics.total_used - statistics.total_freed,
            statistics.total_used, statistics.total_freed,
            statistics.allocation_num, statistics.dealllocation_num);
    // closelog
}

// my malloc
void *malloc(size_t size)
{
    fprintf(stderr, "calling malloc\n");
    statistics.allocation_num++;
    statistics.total_used = statistics.total_used + size;

    void *memory = functions.real_malloc(size + SERVICE_LENGTH);
    if (!memory) {
        statistics.errors++;
        return NULL;
    }
    initialise_service_block(memory, size);

    return memory + SERVICE_LENGTH;
}

// my calloc
void *calloc(size_t num, size_t size)
{
    fprintf(stderr, "calling calloc\n");
    statistics.allocation_num++;
    statistics.total_used = statistics.total_used + (num * size);

    void *memory = functions.real_calloc(num * size + SERVICE_LENGTH, 1);

    if (!memory) {
        statistics.errors++;
        return NULL;
    }
    initialise_service_block(memory, num * size);

    return memory + SERVICE_LENGTH;
}

// my realloc
void *realloc(void *ptr, size_t size)
{
    //what if *ptr is NULL -> do smth about it
    fprintf(stderr, "calling realloc\n");

    if (ptr == NULL) {
        return malloc(size);
    }

    ptr = ptr - SERVICE_LENGTH;

    if (check_service_validity(ptr) != 0) {
        fprintf(stderr, "service struct missing");
        statistics.errors++;
        return NULL;
    }

    size_t block_size = ((struct service_info *)ptr)->block_size;

    struct service_info * memory = functions.real_realloc(ptr, size + SERVICE_LENGTH);
    if (!memory) {
        statistics.errors++;
        return NULL;
    }

    if (ptr == memory) { //was able to extend / contract space used
        fprintf(stderr, "are same\n");
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
    return (void *) memory + SERVICE_LENGTH;
}

// my free
void free(void *ptr)
{
    fprintf(stderr, "calling free\n");

    if (ptr == NULL) {
        return;
    }

    ptr = ptr - SERVICE_LENGTH;
    if (check_service_validity(ptr) != 0) {
        fprintf(stderr, "service struct missing");
        statistics.errors++;
        return;
    }

    statistics.dealllocation_num++;
    statistics.total_freed += ((struct service_info *)ptr)->block_size;
    functions.real_free(ptr);
}
