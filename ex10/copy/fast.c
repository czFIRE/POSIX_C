#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <error.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int copy_fd(int infd, int outfd)
{
    struct stat srcinfo;
    if (fstat(infd, &srcinfo) != 0) {
        error(0, errno, "failed to fstat source file");
        return 0;
    }

    size_t size = srcinfo.st_size;
    ssize_t copied;

    off_t offset = 0;
    while ((copied = sendfile(outfd, infd, &offset, size)) > 0)
        size -= copied;

    if (copied == -1) {
        error(0, errno, "sendfile failed");
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
        error(0, errno, "%s", argv[1]);
        goto main_exit;
    }

    mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP;
    if ((dst = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, mode)) == -1) {
        error(0, errno, "%s", argv[2]);
        goto main_src_close;
    }

    if (copy_fd(src, dst))
        status = EXIT_SUCCESS;

    if (close(dst) != 0)
        error(0, errno, "failed to close %s", argv[2]);

main_src_close:
    if (close(src) != 0)
        error(0, errno, "failed to close %s", argv[1]);

main_exit:
    return status;
}
