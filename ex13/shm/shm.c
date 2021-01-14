#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <error.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define NICE 69
#define BUFFER_LENGTH 80

int main(int argc, char *argv[])
{
    if (argc == 1) {
        fprintf(stderr, "Expected arguments\n");
        return EXIT_FAILURE;
    }

    key_t key = ftok(argv[0], NICE);

    if (key == -1)
        error(EXIT_FAILURE, errno, "ftok(%s, %d)", argv[0], NICE);

    int shmid;
    if ((shmid = shmget(key, BUFFER_LENGTH,
                        IPC_CREAT | IPC_EXCL | S_IRUSR | S_IWUSR)) == -1) {
        shmid = shmget(key, BUFFER_LENGTH, S_IRUSR | S_IWUSR);
        if (shmid == -1) {
            error(EXIT_FAILURE, errno, "shmget");
        }
        printf("Accessed: %d\n", shmid);
    } else {
        printf("Created new: %d\n", shmid);
    }

    char *mem = shmat(shmid, NULL, 0);
    if (mem == (void *)-1) {
        error(0, errno, "shmat");
        goto cleanup_shm;
    }

    if (strcmp(argv[1], "--stop") == 0) {
        if (shmctl(shmid, IPC_RMID, NULL) == -1)
            error(0, errno, "shmctl");

        *mem = 1;
        goto cleanup_shm;
    }

    strncpy(mem + 1, argv[1], BUFFER_LENGTH - 1);

    static char time_buf[16];
    time_t stamp;

    while (1) {
        stamp = time(NULL);
        assert(strftime(time_buf, sizeof(time_buf), "%T", localtime(&stamp)) >
               0);
        printf("%s %s\x1b[0K\r", time_buf, mem + 1);
        fflush(stdout);

        if (*mem != 0) {
            puts("finished");
            break;
        }

        sleep(1);
    }

cleanup_shm:
    if (mem != (void *)-1 && shmdt(mem) == -1)
        error(0, errno, "shmdt");

    return EXIT_SUCCESS;
}
