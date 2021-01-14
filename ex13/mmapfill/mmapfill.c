#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <error.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "usage: %s FILE BLOCKS\n", argv[0]);
        return EXIT_FAILURE;
    }

    // parse number
    char *endp;
    size_t blocks = strtol(argv[2], &endp, 10);
    if (*endp != '\0') {
        fprintf(stderr, "%s: invalid number '%s'\n", argv[0], argv[2]);
        return EXIT_FAILURE;
    }

    size_t pagesize = (size_t) sysconf(_SC_PAGESIZE);

    // this is "blocks * pagesize > 1G", but handles overflow better
    if (blocks > 1024u * (1024u * 1024u / pagesize)) {
        fprintf(stderr, "%s: whoa, calm down there, Satan\n", argv[0]);
        return EXIT_FAILURE;
    }

    size_t length = pagesize * blocks;

    // open file
    int fd;
    int status = EXIT_FAILURE;
    if ((fd = open(argv[1], O_CREAT | O_RDWR, S_IRUSR | S_IWUSR)) == -1)
        error(EXIT_FAILURE, errno,  "%s", argv[1]);

    // change file size
    if (ftruncate(fd, length) == -1) {
        error(0, errno, "truncate %s", argv[1]);
        goto main_close_fd;
    }

    // mmap file
    char *mem = mmap(NULL, length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mem == /* (void*) -1 */ MAP_FAILED) {
        error(0, errno, "mmap");
        goto main_close_fd;
    }

    for (size_t ix = 0; ix < length / sizeof(size_t); ++ix)
        ((size_t*) mem)[ix] = ix;

    if (msync(mem, length, MS_ASYNC) == -1)
        error(0, errno, "msync");

    status = EXIT_SUCCESS;

    if (munmap(mem, length) == -1)
        error(0, errno, "munmap");

main_close_fd:
    if (close(fd) == -1)
        error(0, errno, "failed to close %s", argv[1]);

    return status;
}
