#define _GNU_SOURCE

#include <netdb.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define BUF_SIZE 500
#define MAX_DESC 16

int main(int argc, char *argv[])
{
    struct addrinfo hints;
    struct addrinfo *result, *rp;

    if (argc != 2) {
        fprintf(stderr, "Usage: %s port\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;     /* Allow IPv4 or IPv6 */
    hints.ai_socktype = SOCK_STREAM; /* Datagram socket */
    hints.ai_flags = AI_PASSIVE;     /* For wildcard IP address */
    hints.ai_protocol = 0;           /* Any protocol */
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    int sfd, s;
    s = getaddrinfo(NULL, argv[1], &hints, &result);
    if (s != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(s));
        exit(EXIT_FAILURE);
    }

    /* getaddrinfo() returns a list of address structures.
        Try each address until we successfully bind(2).
        If socket(2) (or bind(2)) fails, we (close the socket
        and) try the next address. */

    struct pollfd list_addr[MAX_DESC];
    for (size_t i = 0; i < MAX_DESC; i++) {
        list_addr[i].fd = -1;
        list_addr[i].events = POLLIN;
    }

    size_t free_addr = 4;

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (sfd == -1)
            continue;

        if (bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0) {
            if (free_addr > 0) {
                if (listen(sfd, 0) == -1) {
                    perror("Listen");
                    close(sfd);
                    continue;
                }

                free_addr--;
                list_addr[free_addr].fd = sfd;

            } else {
                break;
            }
        }

        close(sfd);
    }

    if (rp == NULL) { /* No address succeeded */
        fprintf(stderr, "Could not bind\n");
        exit(EXIT_FAILURE);
    }

    freeaddrinfo(result); /* No longer needed */

    /* Read datagrams and echo them back to sender */

    /*
    struct sockaddr_storage peer_addr;
    socklen_t peer_addr_len;
    ssize_t nread;
    char buf[BUF_SIZE];
    */

    while (1) {
        int ret_code = poll(list_addr, 16, -1);

        // do the loop through all fds

        if (ret_code > 0) {
            int curr_desc = 0;
            while (list_addr[curr_desc].fd != -1) {
                if (curr_desc >= MAX_DESC) {
                    // return error -> can't store
                    return 1;
                }

                if (list_addr[curr_desc].revents & POLLIN) {
                    // pollin functionality -> accept & shutdown
                    continue;
                }

                if (list_addr[curr_desc].revents & POLLHUP) {
                    // pollhup functionality -> shutdown
                    continue;
                }
            }
        }

        // sorry, I needed to sleep :(

        /* // mirroring server
            peer_addr_len = sizeof(struct sockaddr_storage);
            nread = recvfrom(sfd, buf, BUF_SIZE, 0, (struct sockaddr
           *)&peer_addr, &peer_addr_len); if (nread == -1) continue;

            char host[NI_MAXHOST], service[NI_MAXSERV];

            s = getnameinfo((struct sockaddr *)&peer_addr, peer_addr_len, host,
                            NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICSERV);
            if (s == 0)
                printf("Received %ld bytes from %s:%s\n", (long)nread, host,
                       service);
            else
                fprintf(stderr, "getnameinfo: %s\n", gai_strerror(s));

            if (sendto(sfd, buf, nread, 0, (struct sockaddr *)&peer_addr,
                       peer_addr_len) != nread)
                fprintf(stderr, "Error sending response\n");
        */
    }
}