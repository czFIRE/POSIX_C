#define _GNU_SOURCE

#include <errno.h>
#include <error.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

int big_ugly_blob(char *server_name, bool tcp_mode);

#define BUFF_SIZE 128

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "Wrong amount of args\n");
        return EXIT_FAILURE;
    }

    bool tcp_mode = false;
    if (strcmp("-t", argv[1]) == 0) {
        puts("got tcp");
        tcp_mode = true;
    }

    big_ugly_blob(argv[2], tcp_mode);

    return EXIT_SUCCESS;
}

// I am so sorry for your eyes, but I don't have time to refractor this :(
int big_ugly_blob(char *server_name, bool tcp_mode)
{
    int ret_code = 0;

    struct addrinfo hints = {0};
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_PASSIVE;
    hints.ai_socktype = SOCK_DGRAM;

    struct addrinfo *result, *rp;

    ret_code = getaddrinfo(server_name, "time", &hints, &result);
    if (ret_code != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret_code));
        return 1;
    }

    int sfd;

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1) {
            continue;
        }

        if (connect(sfd, rp->ai_addr, rp->ai_addrlen) != -1) {
            break;
        }
    }

    if (rp == NULL) {
        fprintf(stderr, "Wasn't able to bind\n");
        // magical constant aaaaa
        return 1;
    }

    freeaddrinfo(result);

    // send

    time_t curr_time;
    // struct sockaddr_storage peer_addr;
    // socklen_t peer_addr_len = sizeof(struct sockaddr_storage);

    if (!tcp_mode) {

        if (sendto(sfd, &curr_time, 0, 0, rp->ai_addr, rp->ai_addrlen) != 0) {
            fprintf(stderr, "Error sending null packet\n");
            return 1;
        }

        // recieve

        alarm(5);

        if (recvfrom(sfd, &curr_time, 0, 0, rp->ai_addr, &rp->ai_addrlen) ==
            -1) {
            fprintf(stderr, "Error receiving null packet\n");
            return 1;
        }
    } else {
        rp->ai_addr = NULL;
        rp->ai_addrlen = 0;
    }

    // send
    curr_time = time(0);

    if (sendto(sfd, &curr_time, sizeof(time_t), 0, rp->ai_addr,
               rp->ai_addrlen) != sizeof(time_t)) {
        fprintf(stderr, "Error sending time packet\n");
        return 1;
    }

    // recieve
    time_t recv_time;

    if (recvfrom(sfd, &recv_time, sizeof(time_t), 0, rp->ai_addr,
                 &rp->ai_addrlen) == -1) {
        fprintf(stderr, "Error receiving time packet\n");
        return 1;
    }

    printf("Time: %s", ctime(&recv_time));
    close(sfd);

    return 0;
}