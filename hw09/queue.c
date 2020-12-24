#include "queue.h"

#define _GNU_SOURCE

#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <error.h>

#include <assert.h>
#include <pthread.h>

#ifdef DEBUG
#include <stdio.h>
#endif

//--[  Constants and Defines  ]-----------------------------------------------

struct queue_memory {
    char *memory;
    size_t queue_size;

    size_t read_pointer;
    size_t write_pointer;
};

struct queue_synchronization {
    pthread_mutex_t queue_mutex;
    pthread_cond_t cond_push;
    pthread_cond_t cond_pop;
    bool aborted;
};

struct queue {
    size_t queue_capacity;
    size_t elem_size;
    pthread_key_t lib_errno;

    struct queue_memory buffer;
    struct queue_synchronization sync;
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
    if (qptr == NULL) {
        return WRONG_QUEUE_ARG;
    }

    int retval = QUEUE_SUCCESS;

    *qptr = calloc(1, sizeof(struct queue));
    if (*qptr == NULL) {
        retval = ALLOC_FAILURE;
        goto queue_alloc;
    }

    (*qptr)->queue_capacity = queue_capacity;
    (*qptr)->elem_size = elem_size;

    (*qptr)->buffer.memory = calloc(queue_capacity, elem_size);
    if ((*qptr)->buffer.memory == NULL) {
        retval = ALLOC_FAILURE;
        goto buffer_alloc;
    }

    if (pthread_key_create(&(*qptr)->lib_errno, NULL) != 0) {
        retval = KEY_ERROR;
        goto key_init;
    }

    // create synchronization elements
    if (pthread_mutex_init(&(*qptr)->sync.queue_mutex, NULL) != 0) {
        retval = MUTEX_INIT_FAIL;
        goto mutex_init;
    }

    if (pthread_cond_init(&(*qptr)->sync.cond_pop, NULL) != 0) {
        retval = COND_INIT_FAIL;
        goto cond_init_pop;
    }

    if (pthread_cond_init(&(*qptr)->sync.cond_push, NULL) != 0) {
        retval = COND_INIT_FAIL;
        goto cond_init_push;
    }

    goto finish;

cond_init_push:
    pthread_cond_destroy(&(*qptr)->sync.cond_pop);
cond_init_pop:
    pthread_mutex_destroy(&(*qptr)->sync.queue_mutex);
mutex_init:
    pthread_key_delete((*qptr)->lib_errno);
key_init:
    free((*qptr)->buffer.memory);
buffer_alloc:
    free(*qptr);
queue_alloc:
    *qptr = NULL;
finish:
    return retval;
}

int queue_destroy(struct queue *queue)
{
    // close synchronization elements

    if (pthread_mutex_destroy(&queue->sync.queue_mutex) != 0) {
        return MUTEX_DESTROY_FAIL;
    }

    if (pthread_cond_destroy(&queue->sync.cond_push) != 0 ||
        pthread_cond_destroy(&queue->sync.cond_pop) != 0) {
        return COND_DESTROY_FAIL;
    }

    free(queue->buffer.memory);
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
    return queue->buffer.queue_size;
}

bool queue_is_empty(const struct queue *queue)
{
    return queue->buffer.queue_size == 0;
}

bool queue_is_full(const struct queue *queue)
{
    return queue->buffer.queue_size == queue->queue_capacity;
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

    // crit
    assert(pthread_mutex_lock(&queue->sync.queue_mutex) == 0);

    while (queue_is_full(queue)) {
        if (queue->sync.aborted) {
            assert(pthread_mutex_unlock(&queue->sync.queue_mutex) == 0);
            return QUEUE_ABORT;
        }

        pthread_cond_wait(&queue->sync.cond_push, &queue->sync.queue_mutex);
    }

    memcpy(queue->buffer.memory +
               (queue->buffer.write_pointer * queue->elem_size),
           elem, queue->elem_size);

    queue->buffer.write_pointer =
        (queue->buffer.write_pointer + 1) % queue->queue_capacity;
    queue->buffer.queue_size += 1;

    pthread_cond_signal(&queue->sync.cond_pop);

    assert(pthread_mutex_unlock(&queue->sync.queue_mutex) == 0);
    // crit

    return QUEUE_SUCCESS;
}

int queue_pop(struct queue *queue, void *elem)
{

    // crit -> check if queue isn't empty
    assert(pthread_mutex_lock(&queue->sync.queue_mutex) == 0);

    while (queue_is_empty(queue)) {
        if (queue->sync.aborted) {
            assert(pthread_mutex_unlock(&queue->sync.queue_mutex) == 0);
            return QUEUE_ABORT;
        }

        pthread_cond_wait(&queue->sync.cond_pop, &queue->sync.queue_mutex);
    }

    if (elem != NULL) {
        memcpy(elem,
               queue->buffer.memory +
                   (queue->buffer.write_pointer * queue->elem_size),
               queue->elem_size);
    }

    queue->buffer.read_pointer =
        (queue->buffer.read_pointer + 1) % queue->queue_capacity;
    queue->buffer.queue_size -= 1;

    pthread_cond_signal(&queue->sync.cond_push);

    assert(pthread_mutex_unlock(&queue->sync.queue_mutex) == 0);
    // crit

    return QUEUE_SUCCESS;
}

int queue_try_push(struct queue *queue, const void *elem)
{
    if (elem == NULL) {
        return WRONG_QUEUE_ARG;
    }

    // crit -> trylock
    if (pthread_mutex_trylock(&queue->sync.queue_mutex) != 0) {
        return CANT_LOCK;
    }

    if (queue_is_full(queue)) {
        assert(pthread_mutex_unlock(&queue->sync.queue_mutex) == 0);
        return QUEUE_EMPTY;
    }

    memcpy(queue->buffer.memory +
               (queue->buffer.write_pointer * queue->elem_size),
           elem, queue->elem_size);

    queue->buffer.write_pointer =
        (queue->buffer.write_pointer + 1) % queue->queue_capacity;
    queue->buffer.queue_size += 1;

    pthread_cond_signal(&queue->sync.cond_pop);

    assert(pthread_mutex_unlock(&queue->sync.queue_mutex) == 0);
    // crit

    return QUEUE_SUCCESS;
}

int queue_try_pop(struct queue *queue, void *elem)
{
    // crit -> check if queue isn't empty
    if (pthread_mutex_trylock(&queue->sync.queue_mutex) != 0) {
        return CANT_LOCK;
    }

    if (queue_is_empty(queue)) {
        assert(pthread_mutex_unlock(&queue->sync.queue_mutex) == 0);
        return QUEUE_EMPTY;
    }

    if (elem != NULL) {
        memcpy(elem,
               queue->buffer.memory +
                   (queue->buffer.write_pointer * queue->elem_size),
               queue->elem_size);
    }

    queue->buffer.read_pointer =
        (queue->buffer.read_pointer + 1) % queue->queue_capacity;
    queue->buffer.queue_size -= 1;

    pthread_cond_signal(&queue->sync.cond_push);

    assert(pthread_mutex_unlock(&queue->sync.queue_mutex) == 0);
    // crit

    return QUEUE_SUCCESS;
}

//--[  Utilities  ]------------------------------------------------------------

int queue_abort(struct queue *queue)
{
    // maybe some shared attribute to end with error

    // this should probably be in mutex so I know, that this has priority
    queue->sync.aborted = true;
    pthread_cond_broadcast(&queue->sync.cond_pop);
    pthread_cond_broadcast(&queue->sync.cond_push);
    return QUEUE_SUCCESS;
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

    // switch error_code: -> that enum
}

#ifdef DEBUG
void queue_state_debug(const struct queue *queue)
{
    printf("queue memory:\t");
    for (size_t i = 0; i < queue->queue_capacity * queue->elem_size; i++) {
        printf(" %d", queue->buffer.memory[i]);
    }
    putchar('\n');
    printf("queue size:\t\t%ld\n", queue->buffer.queue_size);
    printf("queue read ptr:\t\t%ld\n", queue->buffer.read_pointer);
    printf("queue write ptr:\t%ld\n", queue->buffer.write_pointer);
    putchar('\n');
}
#endif