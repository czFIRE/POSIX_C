#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>

#include <errno.h>
#include <error.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>

#define BUFFER_SIZE 128

int main(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stderr, "gib some args");
        return EXIT_FAILURE;
    }

    if (argc > 10) {
        fprintf(stderr, "I am using variable length array and you can't expect "
                        "me to handle so many args");
        return EXIT_FAILURE;
    }

    struct pollfd fds[argc - 1];

    // open all files for reading and nonblock
    for (size_t i = 0; i < argc - 1; i++) {
        if ((fds[i].fd = open(argv[i + 1], O_RDONLY | O_NONBLOCK)) == -1) {
            error(EXIT_FAILURE, errno, "open(%s)", argv[i + 1]);
        }
    }

    // put these FD into an array of struct pollfd

    // add correct events to "pollfd.events" to watch out for => mostly POLLIN,
    // POLLOUT, POLLHUP

    // skipped //

    static char buffer[BUFFER_SIZE];
    ssize_t rd = 0, wt = 0;
    // while(poll(fds, argc - 1, 0) != 0) ...
    // go through fds, check .revents for retvalues, handle them
    while (poll(fds, argc - 1, 0) > 0) {
        for (size_t i = 0; i < argc - 1; i++) {
            // can read
            if (fds[i].revents & POLLIN) {
                while (wt >= 0 && (rd = read(fds[i].fd, buffer, BUFFER_SIZE)) > 0) {
                    char *ptr = buffer;

                    while (rd > 0 && (wt = write(STDOUT_FILENO, ptr, rd)) > 0) {
                        rd -= wt;
                        ptr += wt;
                    }
                }
            }

            //
            if (fds[i].revents & POLLOUT) {
                // nothing I guess
            }

            // hope none of these can happen at the same time, making sure, that these FD's are invalid
            // should close them first tho
            if (fds[i].revents & POLLERR) {
                fds[i].fd = -1;
            }

            if (fds[i].revents & POLLHUP) {
                fds[i].fd = -1;
            }

            if (fds[i].revents & POLLNVAL) {
                fds[i].fd = -1;
            }
        }
    }

    // if pollin => read until 0

    // close all file descriptors

    // why the fuck AM I SUCH A DISGRACE WHO DOESN'T FUCKING UNDERSTAND THINGS
    return EXIT_SUCCESS;
}