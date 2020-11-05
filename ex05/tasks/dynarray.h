#ifndef DYNARRAY
#define DYNARRAY

#include <stddef.h>

#define ARRAY_TYPE int

void add(ARRAY_TYPE value);
void free_memory(void);
size_t get_length(void);
ARRAY_TYPE *get_array();

#endif