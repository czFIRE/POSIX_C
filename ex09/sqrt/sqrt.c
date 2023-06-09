#define _GNU_SOURCE

#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <error.h>
#include <pthread.h>
#include <unistd.h>

// sqrt 5 without mods => 75 errors
// closely together => 18 errors
// only read lock => 8 errors

// bonus -> přidat atribut zapisovatele, že ho preferujeme NONRECURSIVE_NP

struct common_thread_data {
    unsigned id;
    pthread_t thread;
};

struct shared_thread_data {
    pthread_rwlock_t *lock;

    // problem is here, we need to signalise, that
    volatile bool finished;

    volatile struct {
        long s;
        long n;
    };
};

struct consumer_data {
    struct common_thread_data common;
    struct shared_thread_data *shared;

    long errors;
};

struct producer_data {
    struct common_thread_data common;
    struct shared_thread_data *shared;

    long iterations;
};

pthread_rwlock_t finished_rwlock = PTHREAD_RWLOCK_INITIALIZER;

// Fun fact, there is an ancient (and unfortunately, long abandoned) OS
// experiment called “Plan 9”¹ which had its own C extensions. These
// are today available in GCC with ⟨-fplan9-extensions⟩ and would
// allow the code above to simply use
//
//     struct consumer_data {
//         struct common_thread_data;
//         …
//     };
//
// which would make ⟨struct common_thread_data⟩ a part of the enclosing
// struct, and it work like this:
//
//     struct consumer_data cd;
//     cd.id = …;
//     cd.thread = …;
//
// Too bad that only direct anonymous structs got into C11 :-/
//
// ¹ Yes, this is a direct reference to this film
//   https://www.imdb.com/title/tt0052077/

static void *consumer_run(void *raw_data)
{
    int pterror;

    struct consumer_data *consumer = raw_data;

    // reader thread should not modify data if not necessary, so we
    // will wrap the pointer to shared state in const
    const struct shared_thread_data *state = consumer->shared;

    consumer->errors = 0;
    // race cond here
    if ((pterror = pthread_rwlock_rdlock(&finished_rwlock)) != 0) {
        error(0, pterror, "wrlock");
    }
    bool cond = !consumer->shared->finished;
    if ((pterror = pthread_rwlock_unlock(&finished_rwlock)) != 0) {
        error(0, pterror, "unlock");
    }

    while (cond) {
        // verify that s is the nearest integral square root
        // of n that does not exceed the actual square root,
        // that is, ⟦s² ≤ n < (s + 1)²⟧

        if ((pterror = pthread_rwlock_rdlock(state->lock)) != 0) {
            error(0, pterror, "wrlock");
        }

        long temp_s = state->s, temp_n = state->n;

        if ((pterror = pthread_rwlock_unlock(state->lock)) != 0) {
            error(0, pterror, "unlock");
        }

        if (temp_s * temp_s > temp_n || temp_n >= (temp_s + 1) * (temp_s + 1)) {
            printf("thread %d: mismatch detected\n", consumer->common.id);
            ++consumer->errors;
            // usleep(1000); // 1 ms to avoid the same error again
        }

        // here the thread would usually use the shared data for
        // some computation

        if ((pterror = pthread_rwlock_rdlock(&finished_rwlock)) != 0) {
            error(0, pterror, "wrlock");
        }
        cond = !consumer->shared->finished;
        if ((pterror = pthread_rwlock_unlock(&finished_rwlock)) != 0) {
            error(0, pterror, "unlock");
        }
    }

    pthread_exit(NULL);
}

static void *producer_run(void *raw_data)
{
    int pterror;

    struct producer_data *producer = raw_data;
    struct shared_thread_data *state = producer->shared;

    for (long i = 0; i < producer->iterations; ++i) {
        // generate some random number
        // state->n = 128 + rand() % 4096;

        // now get the square root by the intentinally dumbest
        // algorithm I was able to concieve
        // for (state->s = state->n; state->s * state->s > state->n;
        // --state->s);
        /* nop, that is how dumb this algorithm is */

        long temp_n = 128 + rand() % 4096;
        long temp_s = temp_n;

        for (; temp_s * temp_s > temp_n; --temp_s)
            ;

        if ((pterror = pthread_rwlock_wrlock(state->lock)) != 0) {
            error(0, pterror, "wrlock");
        }
        state->n = temp_n;
        state->s = temp_s;
        if ((pterror = pthread_rwlock_unlock(state->lock)) != 0) {
            error(0, pterror, "unlock");
        }

        // ok, got the number, sleep for a while so that other threads
        // would be able to check the final result
        printf(u8"producer: commit %ld² ≤ %ld (%ld / %ld)\n", state->s,
               state->n, i, producer->iterations);
        // usleep(5000); // 5 ms to counter that 1 ms sleep in consumer_run()
    }

    // done, introduce race cond
    if ((pterror = pthread_rwlock_wrlock(&finished_rwlock)) != 0) {
        error(0, pterror, "wrlock");
    }
    state->finished = true;
    if ((pterror = pthread_rwlock_unlock(&finished_rwlock)) != 0) {
        error(0, pterror, "unlock");
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        error(0, 0, "usage: %s THREADS", basename(argv[0]));
        return EXIT_FAILURE;
    }

    // turn off buffering for standard output
    setbuffer(stdout, NULL, 0u);

    errno = 0;
    char *endp;

    long threads = strtol(argv[1], &endp, 10);
    if (endp == argv[1] || *endp != 0 || errno != 0 || threads <= 0)
        error(EXIT_FAILURE, errno, "%s: not a positive integer", argv[1]);

    struct producer_data producer = {0};
    struct consumer_data *consumers = malloc(sizeof(*consumers) * threads);
    if (consumers == NULL)
        error(EXIT_FAILURE, errno, "cannot allocate memory for worker threads");

    int status = EXIT_FAILURE;

    int pterrno;

    pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;

    struct shared_thread_data shared_data = {
        .finished = false,
        .lock = &rwlock,
    };
    long errors = 0;

    // run the consumers first, they will see 0² = 0, which is fine
    for (long tx = 0; tx < threads; ++tx) {
        struct consumer_data *consumer = &consumers[tx];
        consumer->common.id = tx;
        consumer->shared = &shared_data;

        if ((pterrno = pthread_create(&consumer->common.thread, NULL,
                                      &consumer_run, consumer)) != 0) {
            error(0, pterrno, "cannot start consumer %ld", tx);
            threads = tx;
            goto join_consumers;
        }
    }

    // run producer
    producer.common.id = -1;
    producer.iterations = 5 * threads;
    producer.shared = &shared_data;

    if ((pterrno = pthread_create(&producer.common.thread, NULL, &producer_run,
                                  &producer)) != 0) {
        error(0, pterrno, "cannot start producer thread");
        goto join_consumers;
    }

    // we only care about consumers
    assert(pthread_detach(producer.common.thread) == 0);

join_consumers:
    for (long tx = 0; tx < threads; ++tx) {
        assert(pthread_join(consumers[tx].common.thread, NULL) == 0);
        errors += consumers[tx].errors;
    }

    if (errors == 0) {
        printf("no errors detected\n");
    } else {
        printf("%ld errors detected\n", errors);
    }

    free(consumers);
    return status;
}
