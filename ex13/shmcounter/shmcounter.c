#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <error.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef MAP_FAILED      /* this could possibly be defined in sys/mmap.h */
#   define MAP_FAILED ((void *) -1)
#endif

#define PROJ_ID 42
#define UNUSED(VAR) ((void) (VAR))

int main(int argc, char *argv[])
{
    UNUSED(argc);

    // generate segment key
    key_t key = ftok(argv[0], PROJ_ID);

    if (key == -1)
        error(EXIT_FAILURE, errno, "ftok(%s, %d)", argv[0], PROJ_ID);

    // get page size
    long sc_page_size = sysconf(_SC_PAGESIZE);
    if (sc_page_size == -1)
        error(EXIT_FAILURE, errno, "sysconf PAGESIZE");

    size_t page_size = (size_t) sc_page_size;

    // get segment ID
    int shmid = shmget(key, page_size, IPC_CREAT | S_IRUSR | S_IWUSR);
    if (shmid == -1)
        error(EXIT_FAILURE, errno, "shmget");

    printf("shmid = %d\n", shmid);
    unsigned *mem = shmat(shmid, NULL, 0);
    if (mem == /* (void*) -1 */ MAP_FAILED) {
        error(0, errno, "shmat");
        goto cleanup_shm;
    }

    ++*mem;
    printf("counter = %u\n", *mem);
    printf("press any <Enter> to continue...");

    getchar();

    --*mem;
    printf("counter = %u\n", *mem);

cleanup_shm:
    if (shmctl(shmid, IPC_RMID, NULL) == -1)
        error(0, errno, "shmctl");

    if (mem != MAP_FAILED && shmdt(mem) == -1)
        error(0, errno, "shmdt");

    return 0;
}
