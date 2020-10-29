#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    //_exit(42); //this doesn't get executed
    printf("started easy\n");
    int *vals = calloc(4, sizeof(int));

    if (!vals) {
        printf("calloc failed\n");
        return EXIT_FAILURE;
    }

    *vals = 5;
    printf("%d\n", vals[0]);

    void *tmp = realloc(vals, 8 * sizeof(int));

    if (!tmp) {
        printf("realloc failed\n");
        free(vals);
    }

    vals = tmp;
    *(vals + 5) = 10;
    printf("%d\n", vals[5]);

    void *tmp1 = realloc(vals, 2048 * sizeof(int));

    if (!tmp1) {
        printf("realloc failed\n");
        free(vals);
    }

    vals = tmp1;

    free(vals);
    printf("free vals\n");
    return EXIT_SUCCESS;
}