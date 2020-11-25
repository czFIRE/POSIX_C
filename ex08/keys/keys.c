#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <error.h>
#include <pthread.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef THREADS
#  define THREADS 5u
#endif

#define INT2PTR(N) (((char*) NULL) + (N))

static pthread_key_t key;

// forward declarations
void* thread_entry(void* param);
void  thread_print(void);

void* thread_entry(void* param)
{
    char text[32];
    pid_t    tid = (pid_t) syscall(SYS_gettid);
    unsigned id  = param - NULL;

    snprintf(text, 32, "%d:%d", id, tid);

    int perrno;
    if ((perrno = pthread_setspecific(key, text)) != 0) {
        error(0, perrno, "pthread_setspecific");
        return NULL;
    }

    thread_print();
    return NULL;
}

void thread_print(void)
{
    char* text = (char*) pthread_getspecific(key);

    if (text == NULL) {
        printf("no thread specific data\n");
    } else {
        printf("thread %s\n", text);
    }
}

int main(void)
{
    int      perrno;
    unsigned threads = THREADS;
    pthread_t tids[threads];

    if ((perrno = pthread_key_create(&key, NULL)) != 0)
        error(EXIT_FAILURE, perrno, "pthread_key_create");

    int status = EXIT_FAILURE;
    for (unsigned i = 0u; i < threads; ++i) {
        perrno = pthread_create(tids + i, NULL, thread_entry, INT2PTR(i));

        if (perrno != 0) {
            error(0, perrno, "pthread_create(%u)", i);
            threads = i;
            goto main_pthread_create_fail;
        }
    }

    status = EXIT_SUCCESS;

main_pthread_create_fail:
    for (unsigned i = 0u; i < threads; ++i) {
        if ((perrno = pthread_join(tids[i], NULL)) != 0)
            error(0, perrno, "pthread_join(%u)", i);
    }

    if ((perrno = pthread_key_delete(key)) != 0)
        error(0, perrno, "pthread_key_delete");

    return status;
}
