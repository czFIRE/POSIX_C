#include "queue.h"

#include <stdlib.h>

#include <error.h>

//--[  Constants and Defines  ]-----------------------------------------------

struct queue {
    size_t queue_capacity;
    size_t elem_size;

    void *memory;
    size_t queue_size;
};

#define UNUSED(VAR) ((void)(VAR))

#define NOT_IMPLEMENTED(...)                                                   \
    do {                                                                       \
        error(EXIT_FAILURE, 0, "%s: Not implemented", __func__);               \
        __builtin_unreachable();                                               \
    } while (0)

//--[  Constructor and Destructor  ]-------------------------------------------

int queue_create(struct queue **qptr, size_t elem_size, size_t queue_capacity)
{
    *qptr = malloc(sizeof(struct queue));
    if (*qptr == NULL) {
        return ALLOC_FAILURE;
    }

    (*qptr)->queue_capacity = queue_capacity;
    (*qptr)->elem_size = elem_size;
    (*qptr)->queue_size = 0;

    (*qptr)->memory = calloc(queue_capacity, elem_size);
    if ((*qptr)->memory == NULL) {
        free(*qptr);
        *qptr = NULL;
        return ALLOC_FAILURE;
    }

    return QUEUE_SUCCESS;
}

int queue_destroy(struct queue *queue)
{
    free(queue->memory);
    free(queue);
    return QUEUE_SUCCESS;
}

//--[  Queries  ]--------------------------------------------------------------

size_t queue_type_size(void) { return sizeof(struct queue); }

size_t queue_capacity(const struct queue *queue)
{
    return queue->queue_capacity;
}

size_t queue_element(const struct queue *queue) { return queue->elem_size; }

size_t queue_size(const struct queue *queue) { return queue->queue_size; }

bool queue_is_empty(const struct queue *queue)
{
    return queue->queue_size == 0;
}

bool queue_is_full(const struct queue *queue)
{
    return queue->queue_size == queue->queue_capacity;
}

ssize_t queue_forecast(const struct queue *queue)
{
    // This is a bonus extension.
    // If you do not wish to implement it, you may leave the following
    // macros here.

    UNUSED(queue);
    NOT_IMPLEMENTED();
}

//--[  Queue manipulation  ]---------------------------------------------------

int queue_push(struct queue *queue, const void *elem)
{
    UNUSED(elem);
    queue->queue_size++;
    return QUEUE_SUCCESS;
}

int queue_pop(struct queue *queue, void *elem)
{
    UNUSED(queue);
    UNUSED(elem);
    NOT_IMPLEMENTED();
}

int queue_try_push(struct queue *queue, const void *elem)
{
    UNUSED(queue);
    UNUSED(elem);
    NOT_IMPLEMENTED();
}

int queue_try_pop(struct queue *queue, void *elem)
{
    UNUSED(queue);
    UNUSED(elem);
    NOT_IMPLEMENTED();
}

//--[  Utilities  ]------------------------------------------------------------

int queue_abort(struct queue *queue)
{
    UNUSED(queue);
    NOT_IMPLEMENTED();
}

int queue_errno(const struct queue *queue)
{
    UNUSED(queue);
    NOT_IMPLEMENTED();
}

size_t queue_strerror(int error_code, char *buffer, size_t maxlen)
{
    UNUSED(error_code);
    UNUSED(buffer);
    UNUSED(maxlen);
    NOT_IMPLEMENTED();
}
