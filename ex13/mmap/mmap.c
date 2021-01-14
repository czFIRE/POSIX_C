#define _GNU_SOURCE

#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <error.h>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define FILE_NAME "sync.txt"

#define BUFFER_LENGTH 80

int main(int argc, char *argv[])
{
    if (argc == 1) {
        fprintf(stderr, "Expected arguments\n");
        return EXIT_FAILURE;
    }

    int fd;
    if ((fd = open(FILE_NAME, O_CREAT | O_RDWR)) == -1)
        error(EXIT_FAILURE, errno, "%s", argv[1]);

    if (ftruncate(fd, BUFFER_LENGTH) == -1) {
        error(0, errno, "truncate %s", argv[1]);
        goto main_close_fd;
    }

    char *mem =
        mmap(NULL, BUFFER_LENGTH, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mem == /* (void*) -1 */ MAP_FAILED) {
        error(0, errno, "mmap");
        goto main_close_fd;
    }

    /*struct flock write_lock = {0};
    write_lock.l_type = F_WRLCK;
    write_lock.l_whence = SEEK_SET;
    write_lock.l_start = 0;
    write_lock.l_len = BUFFER_LENGTH;*/
    // fcntl(fd, F_SETLK, &write_lock);

    if (strcmp(argv[1], "--stop") == 0) {
        // critical

        if (flock(fd, LOCK_EX)) {
            error(0, errno, "flock lock");
            goto main_munmap;
        }
        *mem = 1;
        if (flock(fd, LOCK_UN)) {
            error(0, errno, "flock unlock");
            goto main_munmap;
        }
        // end critical

        if (remove(FILE_NAME)) {
            error(0, errno, "remove file");
        }
        goto main_munmap;
    }

    strncpy(mem + 1, argv[1], BUFFER_LENGTH - 1);

    if (msync(mem, BUFFER_LENGTH, MS_ASYNC) == -1)
        error(0, errno, "msync");

    static char time_buf[16];
    time_t stamp;

    while (1) {
        stamp = time(NULL);
        assert(strftime(time_buf, sizeof(time_buf), "%T", localtime(&stamp)) >
               0);

        // critical
        if (flock(fd, LOCK_SH)) {
            error(0, errno, "flock lock");
            goto main_munmap;
        }
        printf("%s %s\x1b[0K\r", time_buf, mem + 1);
        fflush(stdout);

        if (*mem != 0) {
            puts("finished");
            break;
        }
        if (flock(fd, LOCK_UN)) {
            error(0, errno, "flock unlock");
            goto main_munmap;
        }
        // end critical

        sleep(1);
    }

main_munmap:
    if (munmap(mem, BUFFER_LENGTH) == -1)
        error(0, errno, "munmap");

main_close_fd:
    if (close(fd) == -1)
        error(0, errno, "failed to close %s", argv[1]);

    return EXIT_SUCCESS;
}