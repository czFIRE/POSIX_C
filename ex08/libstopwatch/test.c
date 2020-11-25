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

#define UNUSED(x) ((void)(x))

void * thread_run (void * param) {
    UNUSED(param);

    sleep(rand() % 6);

    int perrno;
    if ((perrno = stopwatch_reset()) != 0) {
        error(0, perrno, "pthread_setspecific");
        return 0;
    }


    for (size_t i = 0; i < 4; i++) {
        stopwatch_elapsed(NULL);
        sleep(1);
    }
    
    stopwatch_elapsed(NULL);

    return NULL;
}

int main (int argc, char * argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Wrong amount of program arguments!\n");
        return EXIT_FAILURE;
    }

    char * ptr;
    int thread_num = strtol(argv[1], &ptr, 10);
    if (ptr == argv[1] || *ptr != '\0' || thread_num <= 0) {
        fprintf(stderr, "Expected positive number!\n");
        return EXIT_FAILURE;
    }

    pthread_t* threads = malloc(sizeof(pthread_t) * thread_num);
    if (threads == NULL)
        error(EXIT_FAILURE, errno, "malloc");

    srand(time(NULL));
    
    int perrno;
    if ((perrno = stopwatch_init()) != 0)
        error(EXIT_FAILURE, perrno, "pthread_key_create");

    for (int i = 0; i < thread_num; i++) {
        if ((perrno = pthread_create(&threads[i], NULL, &thread_run, NULL)) != 0)
            error(EXIT_FAILURE, perrno, "pthread_create");
    }

    for (int i = 0; i < thread_num; i++) {
        if ((perrno = pthread_join(threads[i], NULL)) != 0)
            error(0, perrno, "pthread_join");
    }
    printf("Threads joined\n");

    if ((perrno = stopwatch_destroy()) != 0)
        error(EXIT_FAILURE, perrno, "pthread_key_create");

    printf("Main thread ending!\n");
    free(threads);
    return EXIT_SUCCESS;
}
