#include "dynarray.h"

#include <stdio.h>
#include <stdlib.h>

#define START_CAPACITY 16

struct dynarray {
    size_t capacity;
    size_t size;
    ARRAY_TYPE(*array);
};

static struct dynarray array_state = {0};

int expand(void)
{
    if (array_state.capacity == 0) {
        array_state.capacity = START_CAPACITY;
    } else {
        array_state.capacity *= 2;
    }

    void *tmp =
        realloc(array_state.array, array_state.capacity * sizeof(ARRAY_TYPE));
    if (tmp == NULL) {
        return 1;
    }

    array_state.array = tmp;
    return 0;
}

void push_loc(ARRAY_TYPE value, size_t position)
{
    if (array_state.capacity == 0 && expand() != 0) {
        fprintf(stderr, "Can't expand the array!");
        return;
    }

    if (position == array_state.capacity && expand() != 0) {
        fprintf(stderr, "Can't expand the array!");
        return;
    }

    array_state.array[position] = value;
    array_state.size++;
}

void add(ARRAY_TYPE value) { push_loc(value, array_state.size); }

ARRAY_TYPE *get_array(void)
{
    // should add cannary word so I have certainty, that nobody messes with this
    // pointer
    return array_state.array;
}

size_t get_length(void) { return array_state.size; }

void free_memory(void) { free(array_state.array); }