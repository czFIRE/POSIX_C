#ifndef LIST_H
#define LIST_H

#include <stdbool.h>
#include <stddef.h>

// Opaque declaration
struct list;

bool list_init(struct list **list);
void list_destroy(struct list *list);

size_t list_size(const struct list *list);
bool list_is_empty(const struct list *list);

bool list_push(struct list *list, int value);
bool list_pop(struct list *list, int *value);

#endif // LIST_H
