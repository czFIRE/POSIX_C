#include <errno.h>
#include <stdio.h>

#include <error.h>
#include <unistd.h>

#include "constants.h"
#include "get_crc.h"

bool get_crc32(int fd, crc32 *context)
{
    static uint8_t buffer[BUFFER_SIZE];

    // is the file opened?
    if (fd == -1)
        return false;

    ssize_t size;
    crc32_init(context);

    // read and process all blocks of the file
    while ((size = read(fd, buffer, BUFFER_SIZE)) > 0) {
        crc32_update(context, size, buffer);
    }

    // read might have failed
    if (size < 0) {
        error(0, errno, "read");
        return false;
    }

    crc32_finalize(context);
    return size == 0;
}

