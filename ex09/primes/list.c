#include "list.h"

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

// Linked list

struct node {
    struct node *next;
    int value;
};

struct list {
    struct node *head;
    struct node *tail;
    size_t size;
};

bool list_init(struct list **list)
{
    assert(list != NULL);
    return (*list = calloc(1, sizeof(**list))) != NULL;
}

void list_destroy(struct list *list)
{
    assert(list != NULL);
    while (list_pop(list, NULL))
        /* nop */;
    free(list);
}

size_t list_size(const struct list *list)
{
    assert(list != NULL);
    return list->size;
}

bool list_is_empty(const struct list *list)
{
    assert(list != NULL);
    return list->size == 0u;
}

bool list_push(struct list *list, int value)
{
    struct node *node = malloc(sizeof(*node));
    if (node == NULL)
        return false;

    node->value = value;
    node->next = NULL;

    if (list->head == NULL)
        list->head = node;
    else
        list->tail->next = node;

    list->tail = node;
    ++list->size;
    return true;
}

bool list_pop(struct list *list, int *value)
{
    assert(list != NULL);

    if (list->size == 0)
        return false;

    struct node *rm = list->head;

    list->head = list->head->next;
    if (list->head == NULL)
        list->tail = NULL;

    if (value != NULL)
        *value = rm->value;

    free(rm);
    --list->size;
    return true;
}
