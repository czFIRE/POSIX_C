#include "queue.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <error.h>
#include <pthread.h>
#include <unistd.h>

#define UNUSED(VAR) ((void)(VAR))

#define QUEUE_SIZE 4

void *run_prod(void *var)
{
    struct queue *queue = var;

    int nums[] = {1, 2, 3, 4, 0};

    for (size_t i = 0; i < 4; i++) {
        queue_push(queue, nums + i);
    }

    queue_try_push(queue, nums + 4);

    return NULL;
}

void *run_cons(void *var)
{
    struct queue *queue = var;

    int buffer[] = {0};

    for (size_t i = 0; i < 4 * 2 - 1; i++) {
        queue_pop(queue, buffer);
        printf("Got: %d\n", buffer[0]);
    }

    queue_try_pop(queue, buffer);
    printf("Got: %d\n", buffer[0]);
    queue_try_pop(queue, buffer);
    printf("Got: %d\n", buffer[0]);

    return NULL;
}

static const size_t THREADS = 2;

int main(void)
{
    struct queue *queue;
    queue_create(&queue, sizeof(int), QUEUE_SIZE);

    pthread_t prod[2 * THREADS];
    pthread_t cons[THREADS];

    int perrno;
    for (size_t i = 0; i < THREADS * 2; ++i) {
        if ((perrno = pthread_create(prod + i, NULL, run_prod, queue)) != 0)
            error(EXIT_FAILURE, perrno, "pthread_create(%ld)", i);
    }

    for (size_t i = 0; i < THREADS; ++i) {
        if ((perrno = pthread_create(cons + i, NULL, run_cons, queue)) != 0)
            error(EXIT_FAILURE, perrno, "pthread_create(%ld)", i);
    }

    sleep(5);
    queue_abort(queue);

    for (size_t i = 0; i < THREADS; ++i) {
        if ((perrno = pthread_join(cons[i], NULL)) != 0)
            error(0, perrno, "pthread_join(%ld)", i);
    }
    puts("Consumers joined!");

    for (size_t i = 0; i < THREADS * 2; ++i) {
        if ((perrno = pthread_join(prod[i], NULL)) != 0)
            error(0, perrno, "pthread_join(%ld)", i);
    }
    puts("Producers joined!");

    queue_destroy(queue);

    return EXIT_SUCCESS;
}
