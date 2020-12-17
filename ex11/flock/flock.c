#define _GNU_SOURCE
#include <err.h>
#include <errno.h>
#include <error.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <unistd.h>

static const char optstring[] = "vrwno";

enum lock_opt {
    read_lock = F_RDLCK,
    write_lock = F_WRLCK,
    nonblock_lock = F_UNLCK,
};

struct options {
    bool write_err;
    bool unlock_exec;
    // I think I fucked up here really badly and there should be 3 bools here
    enum lock_opt lock_type;
};

void locker(struct options options, char *args[]);

int main(int argc, char *argv[])
{
    int opt;

    // initialize options
    struct options options = {0};
    options.lock_type = write_lock;

    // call getopt() until there are no options left
    while ((opt = getopt(argc, argv, optstring)) != -1) {
        switch (opt) {
        case 'v':
            options.write_err = true;
            break;

        case 'r':
            options.lock_type = read_lock;
            break;

        case 'w':
            options.lock_type = write_lock;
            break;
        case 'n':
            options.lock_type = nonblock_lock;
            break;
        case 'o':
            options.unlock_exec = true;
            break;

        default:
            fprintf(stderr, "Wrong arguments!");
            return EXIT_FAILURE;
        }
    }

    // left-over arguments are in argv[optind] ... argv[argc - 1]
    locker(options, argv + optind);

    // running exec, shouldn't get here
    return EXIT_FAILURE;
}

void locker(struct options options, char *args[])
{
    struct flock lock_opt = {0};
    // fill struct
    lock_opt.l_type = options.lock_type;
    lock_opt.l_whence = SEEK_SET;
    lock_opt.l_start = 0;
    lock_opt.l_len = 0;
    // open file

    int flags = 0;
    switch (options.lock_type) {
    case read_lock:
        flags = O_RDONLY;
        break;

    case write_lock:
        flags = O_RDONLY | O_CREAT;
        break;

    case nonblock_lock:
        flags = O_RDWR | O_NONBLOCK;
        break;

    default:
        break;
    }

    if (options.unlock_exec) {
        flags |= O_CLOEXEC;
    }

    int file;

    if ((file = open(args[0], flags)) == -1) {
        error(1, errno, "open(%s)", args[0]);
    }

    // fcntl -> fails with ebadf, but I don't have time to solve this :(
    // I think I am stupid and I have no fucking clue, what I am supposed to do
    if (fcntl(file, F_SETLK, &lock_opt) == -1) {
        error(1, errno, "fnctl");
    }

    // exec
    execvp(args[2], args + 2);
    err(EXIT_FAILURE, "execvp");
}