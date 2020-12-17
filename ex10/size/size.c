#define _XOPEN_SOURCE 500

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <error.h>
#include <ftw.h>
#include <unistd.h>

#define UNUSED(VAR) ((void)(VAR))

static unsigned long object_size = 0;
static unsigned long block_num = 0;

int callback(const char *path, const struct stat *sb, int tf, struct FTW *ftwb)
{
    UNUSED(path);
    UNUSED(ftwb);

    // můžeme využít inline funkci a takhle vyřešit problém globalních

    switch (tf) {
    // file where stat failed
    case FTW_NS:
        break;

    // regular file
    case FTW_F: /* fallthrough */
    // directory
    case FTW_DP: /* fallthrough */
    case FTW_D:

    // don't know whether I should also add these

    // directory that could not be read
    case FTW_DNR:

    // link if FTW_PHYS was specified
    case FTW_SL:

    // link to a non-existent file
    case FTW_SLN:
        block_num += sb->st_blocks;
        object_size += sb->st_size;

        break;

    default:
        break;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "usage: %s DIRECTORY|FILE|...\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (nftw(argv[1], callback, 256, FTW_CHDIR | FTW_MOUNT | FTW_PHYS) != 0)
        error(EXIT_FAILURE, errno, "ntfw");

    printf("Size: %ldB\t Blocks: %ld\n", object_size, block_num);

    return EXIT_SUCCESS;
}
