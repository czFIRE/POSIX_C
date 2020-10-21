#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

long fib(long n)
{
    if (n <= 1)
        return 1;

    return fib(n - 1) + fib(n - 2);
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "usage: %s NUMBER\n", argv[0]);
        return EXIT_FAILURE;
    }

    errno = 0;
    char *endp;
    long n = strtoll(argv[1], &endp, 10);

    if (endp == argv[1] || *endp != '\0' || errno != 0 || n < 0) {
        fprintf(stderr, "%s: Invalid number\n", argv[1]);
        return EXIT_FAILURE;
    }

    printf("%ld\n", fib(n));
    return EXIT_SUCCESS;
}
