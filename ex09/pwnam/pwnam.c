#define _GNU_SOURCE

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <error.h>
#include <pthread.h>
#include <pwd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef UIDS
#  define UIDS 2u
#endif

#ifndef USE_USLEEP
#  define USE_USLEEP 1
#endif

pthread_barrier_t barrier;

/*
 * Print user name
 *
 * \param   data        uid
 */
void *print_info(void *data)
{
    int perrno;
    uid_t uid = *((uid_t*) data);
    pid_t tid = (pid_t) syscall(SYS_gettid);

    char name[64];

    // wait at the barrier
    if ((perrno = pthread_barrier_wait(&barrier))
            && perrno != PTHREAD_BARRIER_SERIAL_THREAD) {
        // a single thread should get PTHREAD_BARRIER_SERIAL_THREAD
        // as a result, which is NOT an error
        error(0, perrno, "thread %d: pthread_barrier_wait", tid);
        goto print_info_exit;
    }

    struct passwd *passwd = getpwuid(uid);

#if USE_USLEEP
    usleep(rand() % 50);
#endif

    if (passwd == NULL) {
        error(0, errno, "getpwuid(%d)", uid);
        goto print_info_exit;
    }

    strcpy(name, passwd->pw_name);
    printf("%d: %s\n", uid, passwd->pw_name);

print_info_exit:
    return NULL;
}

/**
 * Print username of UIDs 0 and 1000 concurrently
 */
int main(void)
{
    int perrno;
    int status  = EXIT_FAILURE;

    uid_t uids[UIDS] = { 0, 1000 };
    pthread_t thrds[UIDS];

    // initialize the barrier
    if ((perrno = pthread_barrier_init(&barrier, NULL, UIDS)))
        error(EXIT_FAILURE, perrno, "pthread_barrier_init");

    // create threads
    for (size_t i = 0u; i < UIDS; ++i) {
        if ((perrno = pthread_create(thrds + i, NULL, print_info, uids + i))) {
            error(0, perrno, "pthread_create(%zd)", i);
            goto main_barrier_cleanup;
        }
    }

    // join threads
    for (size_t i = 0u; i < UIDS; ++i) {
        if ((perrno = pthread_join(thrds[i], NULL))) {
            error(0, perrno, "pthread_join(%zd)", i);
            goto main_barrier_cleanup;
        }
    }

    status = EXIT_SUCCESS;

main_barrier_cleanup:
    // destroy barrier on exit
    if ((perrno = pthread_barrier_destroy(&barrier)))
        error(EXIT_FAILURE, perrno, "pthread_barrier_destroy");

    return status;
}
