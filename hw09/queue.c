#include "queue.h"

#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <error.h>

//--[  Constants and Defines  ]-----------------------------------------------

// struct queue_memory {
// };

struct queue {
    size_t queue_capacity;
    size_t elem_size;

    char *memory;
    size_t queue_size;

    size_t read_pointer;
    size_t write_pointer;
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
    *qptr = calloc(1, sizeof(struct queue));
    if (*qptr == NULL) {
        return ALLOC_FAILURE;
    }

    (*qptr)->queue_capacity = queue_capacity;
    (*qptr)->elem_size = elem_size;
    //(*qptr)->queue_size = 0;

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

size_t queue_type_size(void)
{
    // fuck you clang
    return sizeof(struct queue);
}

size_t queue_capacity(const struct queue *queue)
{
    // fuck you clang
    return queue->queue_capacity;
}

size_t queue_element(const struct queue *queue)
{
    // fuck you clang
    return queue->elem_size;
}

size_t queue_size(const struct queue *queue)
{
    // fuck you clang
    return queue->queue_size;
}

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
    if (elem == NULL) {
        return WRONG_QUEUE_ARG;
    }

    // crit -> check if queue isn't full
    memcpy(queue->memory + (queue->write_pointer * queue->elem_size), elem,
           queue->elem_size);

    queue->write_pointer = (queue->write_pointer + 1) % queue->queue_capacity;
    queue->queue_size = (queue->queue_size + 1);
    // crit

    return QUEUE_SUCCESS;
}

int queue_pop(struct queue *queue, void *elem)
{
    // crit -> check if queue isn't empty
    if (elem == NULL) {
        memcpy(elem, queue->memory + (queue->write_pointer * queue->elem_size),
               queue->elem_size);
    }

    queue->read_pointer = (queue->read_pointer + 1) % queue->queue_capacity;
    queue->queue_size = (queue->queue_size - 1);
    // crit

    return QUEUE_SUCCESS;
}

int queue_try_push(struct queue *queue, const void *elem)
{
    if (elem == NULL) {
        return WRONG_QUEUE_ARG;
    }

    // crit -> check if queue isn't full and if so, fail
    memcpy(queue->memory + (queue->write_pointer * queue->elem_size), elem,
           queue->elem_size);

    queue->write_pointer = (queue->write_pointer + 1) % queue->queue_capacity;
    queue->queue_size = (queue->queue_size + 1);
    // crit

    return QUEUE_SUCCESS;
}

int queue_try_pop(struct queue *queue, void *elem)
{
    // crit -> check if queue isn't empty
    if (elem == NULL) {
        memcpy(elem, queue->memory + (queue->write_pointer * queue->elem_size),
               queue->elem_size);
    }

    queue->read_pointer = (queue->read_pointer + 1) % queue->queue_capacity;
    queue->queue_size = (queue->queue_size - 1);
    // crit

    return QUEUE_SUCCESS;
}

//--[  Utilities  ]------------------------------------------------------------

int queue_abort(struct queue *queue)
{
    UNUSED(queue);
    NOT_IMPLEMENTED();
}

int queue_errno(const struct queue *queue)
{
    // uumh, what? :D
    // maybe add "last error code" to data structure => second condvar?
    UNUSED(queue);
    return errno;
}

size_t queue_strerror(int error_code, char *buffer, size_t maxlen)
{
    UNUSED(error_code);
    UNUSED(buffer);
    UNUSED(maxlen);
    NOT_IMPLEMENTED();
}
