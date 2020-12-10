#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <error.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 128
#endif

int write_to_stdout(int infd)
{
    static char buffer[BUFFER_SIZE];

    ssize_t rd = 0, wt = 0;

    while (wt >= 0 && (rd = read(infd, buffer, BUFFER_SIZE)) > 0) {
        char *ptr = buffer;

        while (rd > 0 && (wt = write(STDOUT_FILENO, ptr, rd)) > 0) {
            rd -= wt;
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

    if (argc == 1) {
        if (!write_to_stdout(STDIN_FILENO)) {
            fprintf(stderr, "problems with stdin\n");
            return EXIT_FAILURE;
        }
    } else {
        for (int i = 1; i < argc; i++) {
            int src;
            if ((src = open(argv[i], O_RDONLY)) == -1) {
                error(1, errno, "open(%s)", argv[i]);
            }

            if (!write_to_stdout(src)) {
                fprintf(stderr, "problems with %s\n", argv[i]);
                return EXIT_FAILURE;
            }

            if (close(src) != 0) {
                error(1, errno, "close(%s)", argv[i]);
            }
        }
    }

    return EXIT_SUCCESS;
}