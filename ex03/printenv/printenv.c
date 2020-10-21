#define _GNU_SOURCE
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <error.h>

int main(int argc, char *argv[], char *env[])
{
    if (clearenv() != 0) 
        error(EXIT_FAILURE, errno, "clearenv");

    size_t iter = 0;
    while (env[iter] != NULL) {
        // char * val = getenv(env[iter]);
        // printf("%s=%s\n", env[iter], val);
        printf("%s\n", env[iter]);
        iter++;
    }

    return EXIT_SUCCESS;
}