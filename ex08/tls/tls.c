#include <errno.h>
#include <stdlib.h>
#include <stdio.h>

#include <error.h>
#include <pthread.h>

struct thread_data {
    long id;
    pthread_key_t *key;
    pthread_barrier_t *barrier;
};

void set_thread_data(struct thread_data *target, long id, pthread_key_t *key,
        pthread_barrier_t *barrier)
{
    target->id = id;
    target->key = key;
    target->barrier = barrier;
}

void *run_thread(void *raw_data)
{
    struct thread_data *data = raw_data;

    // local (automatic) storage variable
    long id_local = data->id;
    // static storage (per-process)
    static long id_static;
    id_static = data->id;

    // ⟨__thread⟩ is a NON-STANDARD storage specifier for GCC,
    // this is Thread Local Storage (tls)
    static __thread long id_thread;
    id_thread = data->id;

    // STANDARD Thread Local Storage variable, using ⟨pthread_key_t⟩
    long *id_specific = malloc(sizeof(*id_specific));
    if (id_specific == NULL) {
        error(0, errno, "Cannot allocate storage for thread-specific data");
        pthread_exit(NULL);
    }

    *id_specific = data->id;

    int pterrno;
    if ((pterrno = pthread_setspecific(*data->key, id_specific)) != 0) {
        error(0, pterrno, "Cannot set thread-specific key");
        pthread_exit(NULL);
    }

    // this will stop all threads until all of them call this function
    if ((pterrno = pthread_barrier_wait(data->barrier)) != 0
            && pterrno != PTHREAD_BARRIER_SERIAL_THREAD) {
        error(0, pterrno, "Barrier wait failed");
        pthread_exit(NULL);
    }

    // all threads are woken up here at the (almost) same time
    printf("thread %-3ld |  local=%-3ld  static=%-3ld  __thread=%-3ld  pthread_key=%-3ld\n",
            data->id, id_local, id_static, id_thread,
            *(long *) pthread_getspecific(*data->key));

    return NULL;
}

int main(void)
{
    pthread_t thread[5];
    const size_t threads = sizeof(thread) / sizeof(*thread);

    struct thread_data *_thread_data = malloc((sizeof(*_thread_data)) * (threads + 1));
    if (_thread_data == NULL)
        error(EXIT_FAILURE, errno, "malloc");

    // This is a nasty trick to allow thread_data[-1] to represent the main
    // thread. Do not try this at home without parents' supervision.
    struct thread_data *thread_data = &_thread_data[1];

    int pterrno;
    pthread_key_t key;
    if ((pterrno = pthread_key_create(&key, &free)) != 0)
        error(EXIT_FAILURE, errno, "Cannot create thread key");

    // Barrier will be explained later. For now, it is a synchronization
    // element that will block threads until the specified number of them
    // calls ⟨pthread_barrier_wait()⟩ somewhere.
    pthread_barrier_t barrier;
    if ((pterrno = pthread_barrier_init(&barrier, NULL, threads + 1)) != 0)
        error(EXIT_FAILURE, pterrno, "Cannto create barrier");

    set_thread_data(&thread_data[-1], -1, &key, &barrier);

    for (unsigned i = 0; i < threads; ++i) {
        set_thread_data(&thread_data[i], i, &key, &barrier);

        if ((pterrno = pthread_create(&thread[i], NULL, &run_thread, &thread_data[i])) != 0)
            error(EXIT_FAILURE, pterrno, "Cannot start thread %u", i);
    }

    // run main thread as a common thread
    run_thread(&thread_data[-1]);

    // cleanup
    for (unsigned i = 0; i < threads; ++i) {
        if ((pterrno = pthread_join(thread[i], NULL)) != 0)
            error(0, pterrno, "Cannot join thread %u", i);
    }

    if ((pterrno = pthread_barrier_destroy(&barrier)) != 0)
        error(0, pterrno, "Cannot destroy barrier");

    if ((pterrno = pthread_key_delete(key)) != 0)
        error(0, pterrno, "Cannot delete thread key");

    free(_thread_data);
    return EXIT_SUCCESS;
}
