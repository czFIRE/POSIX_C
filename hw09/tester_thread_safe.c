#include "queue.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <error.h>
#include <pthread.h>
#include <unistd.h>

#define UNUSED(VAR) ((void)(VAR))

#define QUEUE_SIZE 2

void *run_thread(void *var)
{
    size_t thread_num = (size_t)var;

    int nums[] = {1, 2, 3};
    int ret_nums[3];

    struct queue *queue;
    queue_create(&queue, sizeof(int), QUEUE_SIZE);
    printf("%d\n", queue_try_pop(queue, ret_nums));

    if (thread_num == 0) {
        // this thread is supposed to hang up
        for (size_t i = 0; i < 3; i++) {
            printf("Thread %ld: push %ld\n", thread_num, i);
            queue_push(queue, nums + i);
        }

    } else {
        for (size_t i = 0; i < 2; i++) {
            printf("Thread %ld: push %ld\n", thread_num, i);
            queue_push(queue, nums + i);
        }
    }

    queue_abort(queue);
    queue_abort(queue);
    queue_abort(queue);
    queue_destroy(queue);
    printf("Thread %ld: finished\n", thread_num);
    return NULL;
}

static const size_t THREADS = 2;

int main(void)
{
    pthread_t t[THREADS];

    int perrno;
    for (size_t i = 0; i < THREADS; ++i) {
        if ((perrno = pthread_create(t + i, NULL, run_thread, (void *)i)) != 0)
            error(EXIT_FAILURE, perrno, "pthread_create(%ld)", i);
    }

    for (size_t i = 0; i < THREADS; ++i) {
        if ((perrno = pthread_join(t[i], NULL)) != 0)
            error(0, perrno, "pthread_join(%ld)", i);
    }
    puts("Threads joined!");

    return EXIT_SUCCESS;
}
