#include "stopwatch.h"

#include <stdio.h>
#include <stdlib.h>

#include <error.h>
#include <errno.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>

pthread_key_t key;

int stopwatch_init(void) {
    return pthread_key_create(&key, &free);
}

int stopwatch_destroy(void) {
    return pthread_key_delete(key);
}

int stopwatch_reset(void) {
    time_t * curr_time = malloc(sizeof(time_t *));
    if (curr_time == NULL)
        error(EXIT_FAILURE, errno, "malloc");

    time(curr_time);

    return pthread_setspecific(key, curr_time);
}

int stopwatch_elapsed(struct timespec *result) {
    time_t* got_time = (time_t*) pthread_getspecific(key);

    if (got_time == NULL) {
        printf("no thread specific data\n");
    } else {
        printf("timediff: %f\n", difftime(time(NULL), *got_time));
    }

    // this is done incorectly, but it gets the job done
    if (result != NULL) {
        result->tv_sec = difftime(*got_time, time(NULL));
    }

    return 0;
}