#include <errno.h>
#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(void)
{
    pid_t pid = fork();

    if (pid == -1) {
        error(EXIT_FAILURE, errno, "fork");
    }

    pid_t own_pid = getpid();

    if (pid > 0) {
#ifdef ENABLE_WAIT
        wait(NULL); // should put something better here
#endif
        printf("(parent)\t%d\t%d\n", own_pid, pid);
    } else {
        printf("(child) \t%d\t%d\n", getppid(), own_pid);
    }

    return EXIT_SUCCESS;
}