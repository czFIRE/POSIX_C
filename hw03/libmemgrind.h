#ifndef LIBMEMGRIND_H
#define LIBMEMGRIND_H

#include <stdbool.h>
#include <stddef.h>

void *malloc(size_t size);
void *calloc(size_t num, size_t size);
void *realloc(void *ptr, size_t size);
void free(void *ptr);

bool can_alloc(size_t size);

#endif // LIBMEMGRIND_H