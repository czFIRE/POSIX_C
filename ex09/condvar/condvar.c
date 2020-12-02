#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include <error.h>
#include <pthread.h>
#include <unistd.h>

#define UNUSED(VAR)             \
    ((void) (VAR))

volatile
int GLOBAL_FLAG = 0;

pthread_mutex_t flag_mutex = PTHREAD_MUTEX_INITIALIZER;

void *run_thread(void *var)
{
    UNUSED(var);

    int flag_is_set = 0;

    while (!flag_is_set) {
        assert(pthread_mutex_lock(&flag_mutex) == 0);
        flag_is_set = GLOBAL_FLAG;
        assert(pthread_mutex_unlock(&flag_mutex) == 0);
    }

    printf("flag is set\n");
    return NULL;
}

static const
int THREADS = 2;

int main(void)
{
    pthread_t t[THREADS];

    int perrno;
    for (int i = 0; i < THREADS; ++i) {
        if ((perrno = pthread_create(t + i, NULL, run_thread, NULL)) != 0)
            error(EXIT_FAILURE, perrno, "pthread_create(%d)", i);
    }

    sleep(5);

    assert(pthread_mutex_lock(&flag_mutex) == 0);
    GLOBAL_FLAG = 1;
    assert(pthread_mutex_unlock(&flag_mutex) == 0);

    for (int i = 0; i < THREADS; ++i) {
        if ((perrno = pthread_join(t[i], NULL)) != 0)
            error(0, perrno, "pthread_join(%d)", i);
    }

    return 0;
}
