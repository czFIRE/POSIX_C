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
    int res, test_size = 2;

    struct queue *queue;

    res = queue_create(&queue, sizeof(size_t), test_size);

    printf("%d\n", queue_is_empty(queue));

    size_t nums[] = {1024, 1025, 1026, 1027};

    queue_state_debug(queue);

    res = queue_push(queue, nums);
    res = queue_try_push(queue, nums + 1);

    queue_state_debug(queue);

    printf("%ld %d\n", queue_size(queue), queue_is_full(queue));

    size_t ret_nums[4] = {0};

    res = queue_pop(queue, ret_nums);

    queue_state_debug(queue);

    res = queue_push(queue, nums + 2);
    puts("pushed");
    res = queue_try_pop(queue, ret_nums + 1);
    puts("popped");

    printf("Got these numbers: ");
    for (size_t i = 0; i < 4; i++) {
        printf("%ld ", ret_nums[i]);
    }
    putchar('\n');

    // res = queue_push(queue, nums + 2);
    res = queue_try_push(queue, nums + 3);

    queue_state_debug(queue);

    queue_destroy(queue);
    puts("Finished!");

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
