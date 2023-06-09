#include "list.h"

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <error.h>
#include <pthread.h>

struct shared_data {
    volatile bool finished;
    struct list *list;
};

struct thread_data {
    long id;
    pthread_t thread;

    struct shared_data *shared;
    long counter;
};

pthread_mutex_t list_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condition = PTHREAD_COND_INITIALIZER;

static inline bool is_prime(int n)
{
    assert(n >= 0);

    // an (intentionally) very dumb and slow implementation
    if (n == 0 || n == 1)
        return false;

    for (int k = 2; k < n; ++k) {
        if (n % k == 0)
            return false;
    }

    return true;
}

void *run_producer(void *ptr)
{
    struct thread_data *data = ptr;
    data->counter = 0;

    for (int k = 1000; k < 5000; ++k) {
        if (!is_prime(k))
            continue;

        //
        assert(pthread_mutex_lock(&list_mutex) == 0);
        bool tmp = !list_push(data->shared->list, k);
        if (tmp) {
            error(0, errno, "producer %ld: failed to push data", data->id);
        }
        else {
            // <= waiting threads
            // if (list_size() == 1) {
            //      signal
            // }
            assert(pthread_cond_signal(&condition) == 0);
            ++data->counter;
        }
        // unlock
        assert(pthread_mutex_unlock(&list_mutex) == 0);
    }

    pthread_exit(NULL);
}

void *run_consumer(void *ptr)
{
    struct thread_data *data = ptr;
    data->counter = 0;

    while (true) {
        int k;
        // .
        assert(pthread_mutex_lock(&list_mutex) == 0);

        while (list_is_empty(data->shared->list)) {
            pthread_cond_wait(&condition, &list_mutex);
            if (data->shared->finished) {
                assert(pthread_mutex_unlock(&list_mutex) == 0);
                pthread_exit(NULL);
            }
        }

        bool tmp = !list_pop(data->shared->list, &k);
        assert(pthread_mutex_unlock(&list_mutex) == 0);

        // lock
        // while (tmp)
        if (tmp) {
            // no data -> gotta check this
            if (data->shared->finished) {
                // … and there will never be more data :/
                // unlock
                break;
            }

            // wait
            // all right, try again
            continue;
        }
        //unlock

        // yey, got a prime!
        ++data->counter;
    }

    pthread_exit(NULL);
}

static const int PRODUCERS = 5;
static const int CONSUMERS = 2;

static bool _run_bulk(size_t count, struct thread_data thread_data[count],
                      size_t *actual, struct shared_data *shared,
                      void *(*function)(void *))
{
    int pterrno;

    for (*actual = 0u; *actual < count; ++*actual) {
        struct thread_data *data = &thread_data[*actual];
        data->id = (long)*actual;
        data->shared = shared;

        if ((pterrno = pthread_create(&data->thread, NULL, function, data)) !=
            0) {
            error(0, pterrno, "cannot create thread %zu", *actual);
            return false;
        }
    }

    return true;
}

int main(void)
{
    struct thread_data *producers = malloc(sizeof(*producers) * PRODUCERS);
    if (producers == NULL)
        error(EXIT_FAILURE, errno, "failed to allocate space for producers");

    struct thread_data *consumers = malloc(sizeof(*consumers) * CONSUMERS);
    if (consumers == NULL)
        error(EXIT_FAILURE, errno, "failed to allocate space for consumers");

    struct list *list;
    if (!list_init(&list))
        error(EXIT_FAILURE, errno, "failed to create list");

    struct shared_data shared = {
        .finished = false,
        .list = list,
    };

    struct {
        long consumed;
        long produced;
    } counter = {0};

    size_t actual_producers, actual_consumers;

    // create consumers first
    if (!_run_bulk(CONSUMERS, consumers, &actual_consumers, &shared,
                   &run_consumer))
        goto join_consumers;

    if (!_run_bulk(PRODUCERS, producers, &actual_producers, &shared,
                   &run_producer))
        goto join_producers;

join_producers:
    //printf("Producers:\n");
    for (size_t tx = 0; tx < actual_producers; ++tx) {
        assert(pthread_join(producers[tx].thread, NULL) == 0);
        counter.produced += producers[tx].counter;
    }

join_consumers:
    //printf("Consumers:\n");
    // lock
    // tell consumers that there will be no more elements
    shared.finished = true;
    // I want to make sure, that all consumers get this "signal"
    pthread_cond_broadcast(&condition);
    // unlock

    for (size_t tx = 0; tx < actual_consumers; ++tx) {
        assert(pthread_join(consumers[tx].thread, NULL) == 0);
        counter.consumed += consumers[tx].counter;
    }

    printf("produced:   %ld elements\n", counter.produced);
    printf("consumed:   %ld elements\n", counter.consumed);

    list_destroy(list);

    free(consumers);
    free(producers);

    return counter.produced == counter.consumed;
}
