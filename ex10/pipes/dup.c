#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <wait.h>

void child(int pipefd[2])
{
    // we don't need the reading handle
    close(pipefd[0]);

    // copy writing end of pipe to stdout
    if (dup2(pipefd[1], STDOUT_FILENO) == -1)
        err(EXIT_FAILURE, "dup2");

    // execute ls
    execlp("ls", "ls", "-l", "/", NULL);
    err(EXIT_FAILURE, "execlp");
}

void parent(int pipefd[2])
{
    // we don't need the writing handle
    close(pipefd[1]);

    // read from the pipe until EOF
    char buffer;
    while (read(pipefd[0], &buffer, 1) > 0)
        write(STDOUT_FILENO, &buffer, 1);

    close(pipefd[0]);

    // wait for the child to exit
    wait(NULL);
}

int main(void)
{
    int pfd[2];
    pipe(pfd);

    pid_t pid = fork();

    if (pid == -1)
        err(EXIT_FAILURE, "fork");

    if (pid == 0)
        child(pfd);
    else
        parent(pfd);

    return EXIT_SUCCESS;
}
