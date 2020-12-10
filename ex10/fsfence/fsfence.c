#define _GNU_SOURCE

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <error.h>
#include <unistd.h>
#include <seccomp.h>
#include <sys/wait.h>

static
void setup_seccomp(void)
{
    int rc;

    // allow all syscalls by default (blacklisting)
    scmp_filter_ctx ctx = seccomp_init(SCMP_ACT_ALLOW);

    if (ctx == NULL)
        error(EXIT_FAILURE, errno, "seccomp_init");

#define SECCOMP_ADD(...)                                \
    do {                                                \
        rc = seccomp_rule_add(ctx, ## __VA_ARGS__);     \
        if (rc < 0)                                     \
            goto seccomp_error;                         \
    } while (0)

    // filesystem operations
    SECCOMP_ADD(SCMP_ACT_ERRNO(EPERM), SCMP_SYS(chmod), 0);
    SECCOMP_ADD(SCMP_ACT_ERRNO(EPERM), SCMP_SYS(unlink), 0);
    SECCOMP_ADD(SCMP_ACT_ERRNO(EPERM), SCMP_SYS(unlinkat), 0);
    // TODO: add more syscalls to filter out

#undef SECCOMP_ADD

    if ((rc = seccomp_load(ctx)) < 0)
        goto seccomp_error;

    seccomp_release(ctx);
    return;

seccomp_error:
    error(0, errno, "failed to set up seccomp");
    seccomp_release(ctx);
    exit(EXIT_FAILURE);
}

static inline
void usage(const char *argv0)
{
    fprintf(stderr, "usage: %s PROGRAM [PARAMETERS]\n", basename(argv0));
}

int main(int argc, char *argv[])
{
    if (argc <= 1) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    setup_seccomp();
    execvp(argv[1], argv + 1);
    error(EXIT_FAILURE, errno, "failed to exec %s", argv[1]);
}
