#ifndef LIBMEMGRIND_H
#define LIBMEMGRIND_H

#include <stddef.h>

struct service_info {
};

struct real_functions {
    void *(*real_malloc)(size_t);
    void *(*real_calloc)(size_t, size_t);
    void *(*real_realloc)(void *, size_t);
    void (*real_free)(void *);
};

void *malloc(size_t size);
void *calloc(size_t num, size_t size);
void *realloc(void *ptr, size_t size);
void free(void *ptr);

#endif // LIBMEMGRIND_H