#define _GNU_SOURCE

#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <error.h>
#include <fcntl.h>
#include <unistd.h>

#include "crc.h"
#include "get_crc.h"

int main(int argc, char *argv[])
{
    int errors = 0;
    char *empty[2] = { "-", NULL };
    char **args = argv + 1;

    // fake standard arguments if none provided
    if (argc == 1)
        args = empty;

    for (char **fn_ptr = args; *fn_ptr != NULL; ++fn_ptr) {
        int fd;

        // open file or use stdin if "-" specified
        if (strcmp(*fn_ptr, "-") == 0) {
            fd = STDIN_FILENO;
        } else {
            fd = open(*fn_ptr, O_RDONLY);

            if (fd == -1) {
                error(0, errno, "%s", *fn_ptr);
                continue;
            }
        }

        // compute CRC32 hash of the given chunk
        crc32 context;
        if (get_crc32(fd, &context)) {
            printf("%08" PRIx32 " %s\n", context.hash, *fn_ptr);
        } else {
            error(0, errno, "get_crc32(%s)", *fn_ptr);
            ++errors;
        }

        // yep, even close() can fail
        if (close(fd) == -1) {
            error(0, errno, "fclose(%s)", *fn_ptr);
            ++errors;
        }
    }

    // that's probably it
    return EXIT_SUCCESS;
}
