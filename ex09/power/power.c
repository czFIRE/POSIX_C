#define _GNU_SOURCE

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <error.h>
#include <pthread.h>

struct shared_worker_data {
    long threads;
    volatile int *counter;
};

struct worker {
    unsigned id;
    pthread_t thread;

    struct shared_worker_data *shared;
};

void *worker_run(void *raw_data)
{
    struct worker *worker = raw_data;

    for (long i = 0; i < worker->shared->threads; ++i) {
        ++*worker->shared->counter;
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

    struct worker *workers = malloc(sizeof(*workers) * threads);
    if (workers == NULL)
        error(EXIT_FAILURE, errno, "cannot allocate memory for worker threads");

    volatile int counter = 0;
    int expected = threads * threads;

    int status = EXIT_FAILURE;

    struct shared_worker_data shared_data = {
        .threads = threads,
        .counter = &counter,
    };

    int pterrno;
    for (long tx = 0; tx < threads; ++tx) {
        struct worker *worker = &workers[tx];
        worker->id = tx;
        worker->shared = &shared_data;

        if ((pterrno = pthread_create(&worker->thread, NULL,
                    &worker_run, worker)) != 0) {
            error(0, pterrno, "cannot start thread %ld", tx);
            threads = tx;
            goto join_threads;
        }
    }

join_threads:
    for (long tx = 0; tx < threads; ++tx) {
        assert(pthread_join(workers[tx].thread, NULL) == 0);
    }

    printf(u8"expected: %ld² = %d\n", threads, counter);
    printf(u8"got:      %ld² = %d\n", threads, expected);

    if (counter == expected) {
        printf("ok\n");
        status = EXIT_SUCCESS;
    } else {
        printf("value mismatch; possible synchronization failure\n");
    }

    free(workers);
    return status;
}
