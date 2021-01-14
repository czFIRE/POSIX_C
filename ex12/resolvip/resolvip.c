#define _GNU_SOURCE

#include <errno.h>
#include <error.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>

// resolv_conf => tam se mi ukládají názvy a tak, usefull k DNS

// ano, může se stát, že DNS je správně a nemusí nám dojít to jméno

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Wrong amount of args\n");
        return EXIT_FAILURE;
    }

    int ret_code = 0;

    struct addrinfo hints = {0};
    hints.ai_family = AF_UNSPEC;
    hints.ai_flags = AI_ALL | AI_CANONNAME; //AI_ALL zbytečné
    hints.ai_socktype = SOCK_DGRAM; //to je jeden z možných, se kterými se mohu připojit

    struct addrinfo *result, *rp;

    ret_code = getaddrinfo(argv[1], NULL, &hints, &result);
    if (ret_code != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret_code));
        return EXIT_FAILURE;
    }

    char *host_name = result->ai_canonname;
    puts(host_name);

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        char host[NI_MAXHOST], service[NI_MAXSERV];

        // vlastně jde o typy soketů, které se tam mohou objevit -> DGRAM, STREAM, RAW

        ret_code = getnameinfo(rp->ai_addr, rp->ai_addrlen, host, NI_MAXHOST,
                               service, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

        if (ret_code != 0) {
            fprintf(stderr, "getnameinfo: %s\n", gai_strerror(ret_code));
            continue;
        }

        char name_host[NI_MAXHOST];

        ret_code = getnameinfo(rp->ai_addr, rp->ai_addrlen, name_host,
                               NI_MAXHOST, NULL, 0, NI_NAMEREQD);

        if (ret_code != 0) {
            fprintf(stderr, "Couldn't resolve hostname getnameinfo: %s\n",
                    gai_strerror(ret_code));
            continue;
        }

        printf("%s: %s\n", name_host, host);
    }

    freeaddrinfo(result);

    return EXIT_SUCCESS;
}