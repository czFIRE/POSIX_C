#include <stdio.h>
#include <stdlib.h>

#include <error.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

void* thread_run(void* unused)
{
    printf("(thread) tid=%d\n", (pid_t) syscall (SYS_gettid));
    printf("(thread) pid=%d\n", getpid());

#if defined(__linux__) && defined(__GLIBC__)
    // This is NOT a portable NOR a correct way to identify a thread!
    // It only exists for demonstration purposes.
    printf("(thread) ptd=%lu\n", (unsigned long) pthread_self());
#endif

    return NULL;
}

int main(void)
{
    pid_t tid = (pid_t) syscall (SYS_gettid);
    printf("(main)   tid=%d\n", tid);
    printf("(main)   pid=%d\n", getpid());

#if defined(__linux__) && defined(__GLIBC__)
    // Again, NOT portable!
    printf("(main)   ptd=%lu\n", pthread_self());
#endif

    int perrno;
    pthread_t thread;
    if ((perrno = pthread_create(&thread, NULL, &thread_run, NULL)) != 0)
        error(EXIT_FAILURE, perrno, "pthread_create");

    printf("thread created\n");

    if ((perrno = pthread_join(thread, NULL)) != 0)
        error(0, perrno, "pthread_join");

    printf("thread joined\n");
    return 0;
}
