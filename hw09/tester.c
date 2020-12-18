#include "queue.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <error.h>
#include <pthread.h>
#include <unistd.h>

#define UNUSED(VAR) ((void)(VAR))

/*volatile int GLOBAL_FLAG = 0;

pthread_mutex_t flag_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition = PTHREAD_COND_INITIALIZER;

void *run_thread(void *var)
{
    UNUSED(var);

    int flag_is_set = 0;

    assert(pthread_mutex_lock(&flag_mutex) == 0);

    while (!flag_is_set) {
        pthread_cond_wait(&condition, &flag_mutex);
        flag_is_set = GLOBAL_FLAG;
    }
    assert(pthread_mutex_unlock(&flag_mutex) == 0);

    printf("flag is set\n");
    return NULL;
}

static const int THREADS = 2;*/

int main(void)
{
    int res;

    struct queue *queue;

    res = queue_create(&queue, sizeof(size_t), 3);

    printf("%d\n", queue_is_empty(queue));

    res = queue_push(queue, NULL);
    res = queue_push(queue, NULL);

    printf("%ld %d\n", queue_size(queue), queue_is_full(queue));

    queue_destroy(queue);

    UNUSED(res);

    /*pthread_t t[THREADS];

    int perrno;
    for (int i = 0; i < THREADS; ++i) {
        if ((perrno = pthread_create(t + i, NULL, run_thread, NULL)) != 0)
            error(EXIT_FAILURE, perrno, "pthread_create(%d)", i);
    }

    sleep(5);

    assert(pthread_mutex_lock(&flag_mutex) == 0);
    GLOBAL_FLAG = 1;
    pthread_cond_broadcast(&condition);
    assert(pthread_mutex_unlock(&flag_mutex) == 0);

    for (int i = 0; i < THREADS; ++i) {
        if ((perrno = pthread_join(t[i], NULL)) != 0)
            error(0, perrno, "pthread_join(%d)", i);
    }*/

    return EXIT_SUCCESS;
}
