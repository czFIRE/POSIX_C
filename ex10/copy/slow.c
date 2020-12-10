#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <error.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef BUFFER_SIZE
#  define BUFFER_SIZE 64
#endif

int copy_fd(int infd, int outfd)
{
    static char buffer[BUFFER_SIZE];

    ssize_t rd = 0, wt = 0;

    while (wt >= 0 && (rd = read(infd, buffer, BUFFER_SIZE)) > 0) {
        char *ptr = buffer;

        while (rd > 0 && (wt = write(outfd, ptr, rd)) > 0) {
            rd  -= wt;
            ptr += wt;
        }
    }

    if (rd == -1) {
        error(0, errno, "read");
        return 0;
    }

    if (wt == -1) {
        error(0, errno, "write");
        return 0;
    }

    return 1;
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "usage: %s SOURCE DESTINATION\n", argv[0]);
        return EXIT_FAILURE;
    }

    int status = EXIT_FAILURE;

    int src, dst;
    if ((src = open(argv[1], O_RDONLY)) == -1) {
        error(0, errno, "open(%s)", argv[1]);
        goto main_exit;
    }

    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;
    if ((dst = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, mode)) == -1) {
        error(0, errno, "open(%s)", argv[2]);
        goto main_src_close;
    }

    if (copy_fd(src, dst))
        status = EXIT_SUCCESS;

    if (close(dst) != 0)
        error(0, errno, "close(%s)", argv[2]);

main_src_close:
    if (close(src) != 0)
        error(0, errno, "close(%s)", argv[1]);

main_exit:
    return status;
}
