#include <err.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <wait.h>

void parent(int pipefd[2])
{
    // we don't need the reading handle
    close(pipefd[0]);

    char data[] = "Hello!";
    write(pipefd[1], data, strlen(data));

    // closing the handle will manifest as EOF on the other end
    close(pipefd[1]);

    // wait for the child to end
    wait(NULL);
}

void child(int pipefd[2])
{
    // we don't need the writing handle
    close(pipefd[1]);

    char buffer;
    while (read(pipefd[0], &buffer, 1) > 0)
        write(STDOUT_FILENO, &buffer, 1);

    // got EOF
    write(STDOUT_FILENO, "\n", 1);

    close(pipefd[0]);
}

int main(void)
{
    int pfd[2];

    if (pipe(pfd) == -1)
        err(EXIT_FAILURE, "pipe");

    pid_t pid = fork();

    if (pid == -1)
        err(EXIT_FAILURE, "fork");

    if (pid == 0)
        child(pfd);
    else
        parent(pfd);

    return EXIT_SUCCESS;
}
