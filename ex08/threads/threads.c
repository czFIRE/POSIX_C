#include <stdio.h>
#include <stdlib.h>

#include <error.h>
#include <errno.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

#define UNUSED(x) ((void)(x))

pid_t gettid(void) {
    return (pid_t) syscall (SYS_gettid);
}

void * thread_run (void * param) {
#if defined(ENABLE_DETACH)
    pthread_detach(pthread_self());
#endif
    UNUSED(param);
    printf("(thread) tid=%d\n", gettid());
    printf("(thread) pid=%d\n", getpid());

#if defined(__linux__) && defined(__GLIBC__)
    // This is NOT a portable NOR a correct way to identify a thread!
    // It only exists for demonstration purposes.
    printf("(thread) ptd=%lu\n", (unsigned long) pthread_self());
#endif

    return NULL;
}

int main (int argc, char * argv[]) {
    #if defined(ENABLE_JOIN) && defined(ENABLE_DETACH)
        haha, error
    #endif

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

    pid_t tid = gettid();
    printf("(main)   tid=%d\n", tid);
    printf("(main)   pid=%d\n", getpid());

#if defined(__linux__) && defined(__GLIBC__)
    // Again, NOT portable!
    printf("(main)   ptd=%lu\n", pthread_self());
#endif

    pthread_t* threads = malloc(sizeof(pthread_t) * thread_num);
    if (threads == NULL)
        error(EXIT_FAILURE, errno, "malloc");

    int perrno;
    for (int i = 0; i < thread_num; i++) {
        if ((perrno = pthread_create(&threads[i], NULL, &thread_run, NULL)) != 0)
            error(EXIT_FAILURE, perrno, "pthread_create");
    }
    
#if defined(ENABLE_JOIN)
    for (int i = 0; i < thread_num; i++) {
        if ((perrno = pthread_join(threads[i], NULL)) != 0)
            error(0, perrno, "pthread_join");
    }
    printf("Threads joined\n");
#endif

    printf("Main thread ending!\n");
    free(threads);
    return EXIT_SUCCESS;
}