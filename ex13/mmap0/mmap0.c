#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <error.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

void test_null(void)
{
    pid_t pid = fork();
    if (pid < 0)
        error(EXIT_FAILURE, errno, "fork");

    // child
    if (pid == 0) {
#ifdef SILENT
        int fd = open("/dev/null", O_WRONLY);
        if (fd == -1)
            error(EXIT_FAILURE, errno, "open");

        if (dup2(fd, STDOUT_FILENO) == -1 || dup2(fd, STDERR_FILENO) == -1)
            error(EXIT_FAILURE, errno, "dup2");
#endif // SILENT

        char *ptr = NULL;
        strcpy(ptr, "Where is your dog now?");
        printf("%s\n", ((char*) NULL));
        exit(EXIT_SUCCESS);
    }

    // parent
    int status;
    if (waitpid(pid, &status, 0) == -1)
        error(EXIT_FAILURE, errno, "wait %d", pid);

    if (WIFSIGNALED(status) && WTERMSIG(status) == SIGSEGV)
        printf("child process died with SIGSEGV\n");
    else if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
        printf("child process terminated with error status %d\n", status);
    else
        printf("child process finished successfully\n");
}

int main(void)
{
    test_null();

    int mmap_prot = PROT_READ | PROT_WRITE;
    int mmap_flags = MAP_SHARED | MAP_ANONYMOUS | MAP_FIXED_NOREPLACE;

    if (mmap(NULL, 4096, mmap_prot, mmap_flags, -1, 0) == (void*) -1)
        error(EXIT_FAILURE, errno, "mmap");

    test_null();

    return EXIT_SUCCESS;
}
