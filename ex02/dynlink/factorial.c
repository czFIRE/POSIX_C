#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <error.h>

#include "fact.h"

int main(int argc, char *argv[])
{
    if (argc == 1) {
        fprintf(stderr, "usage: %s NUMBER [NUMBER...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    char  *endp;
    for (int arg = 1u; arg < argc; ++arg) {
        errno = 0;

        long long user_input = strtoll(argv[arg], &endp, 10);

        if (errno != 0 || *endp != '\0' || user_input < 0)
            error(EXIT_FAILURE, errno, "%s: not a positive integer", argv[arg]);

        number result = factorial((number) user_input);
        printf("%lld! = %u\n", user_input, result);
    }

    return EXIT_SUCCESS;
}
