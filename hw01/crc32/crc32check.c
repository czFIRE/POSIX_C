#define _GNU_SOURCE

#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <error.h>
#include <fcntl.h>
#include <unistd.h>

#include "crc.h"
#include "get_crc.h"

int process_bundle(int fd, char *fn)
{
    int errors = 0;
    int lineno = 0;

    ssize_t bytes = 0;
    size_t n = 0U;
    char *lineptr = NULL;

    FILE *wrap = fdopen(fd, "rb");

    if (!wrap)
        return 0;

    // each line describes a file and its hash
    while ((bytes = getline(&lineptr, &n, wrap)) > 0) {
        ++lineno;

        // line format: "CRCSTR FILENAME"
        char *crcstr = strtok(lineptr, " \n");
        char *name  = strtok(NULL,    "\n");
        char *endp;

        crc32 context;
        uint32_t crc;

        if (name == NULL || crcstr == NULL) {
            error(0, 0, "%s:%d: invalid format", fn, lineno);
            ++errors;
            break;
        }

        // this is a turbo hack; don't tell my mother I wrote this
        if ((crc = strtoull(crcstr, &endp, 16)), (*endp != '\0')) {
            error(0, 0, "%s:%d: invalid hash '%s'", fn, lineno, crcstr);
            ++errors;
            break;
        }

        // compute CRC32 of the target file
        int fd_other = open(name, O_RDONLY);
        if (fd_other == -1) {
            error(0, errno, "%s", name);
            ++errors;
            continue;
        }

        // verify CRC32
        if (!get_crc32(fd_other, &context)) {
            error(0, errno, "%s", name);
            ++errors;
        } else if (crc == context.hash) {
            printf("%s: OK\n", name);
        } else {
            printf("%s: FAILED\n", name);
            ++errors;
        }

        if (close(fd_other))
            error(0, errno, "%s", name);
    }

    if (errors)
        printf("WARNING: there were %d errors\n", errors);

    return errors;
}

int main(int argc, char *argv[])
{
    int fd;
    char *fn = argv[1];

    if (argc == 1) {
        fd = STDIN_FILENO;
        fn = "stdin";
    } else if (argc == 2) {
        if ((fd = open(argv[1], O_RDONLY)) == -1)
            error(EXIT_FAILURE, 0, "%s", argv[1]);
    } else {
        fprintf(stderr, "usage: %s [BUNDLE]\n", argv[0]);
        return EXIT_FAILURE;
    }

    int errors = process_bundle(fd, fn);

    if (fd != STDIN_FILENO && close(fd))
        error(EXIT_FAILURE, errno, "%s", argv[1]);

    return errors ? EXIT_FAILURE : EXIT_SUCCESS;
}
