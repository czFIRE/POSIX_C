#define _XOPEN_SOURCE 500

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <error.h>
#include <ftw.h>
#include <unistd.h>

#define SGR_RED     31
#define SGR_BLUE    34
#define SGR_YELLOW  33
#define SGR_CYAN    36

#define UNUSED(VAR) ((void) (VAR))

int callback(const char *path, const struct stat *sb, int tf, struct FTW *ftwb)
{
    UNUSED(sb);
    printf("%*s", ftwb->level * 2, "");

    char color = '0', type;
    switch (tf) {
    // regular file
    case FTW_F:
        type  = 'f';
        break;

    // directory
    case FTW_DP:    /* fallthrough */
    case FTW_D:
        type  = 'd';
        color = SGR_BLUE;
        break;

    // directory that could not be read
    case FTW_DNR:
        type  = 'D';
        color = SGR_YELLOW;
        break;

    // link if FTW_PHYS was specified
    case FTW_SL:
        type  = 'l';
        color = SGR_CYAN;
        break;

    // link to a non-existent file
    case FTW_SLN:
        type  = 'L';
        color = SGR_RED;
        break;

    // file where stat failed
    case FTW_NS:
        type  = '?';
        color = SGR_YELLOW;
        break;
    }

    printf("%c \x1b[%dm%s\x1b[0m\n", type, color, path + ftwb->base);
    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "usage: %s DIRECTORY\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (nftw(argv[1], callback, 256, FTW_CHDIR | FTW_MOUNT | FTW_PHYS) != 0)
        error(EXIT_FAILURE, errno, "ntfw");

    return EXIT_SUCCESS;
}
