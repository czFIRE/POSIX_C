#include <stdlib.h>
#include <stdio.h>

int main(void) {
    printf("started easy\n");
    int * vals = calloc(4, sizeof(int));

    if (!vals) {
        printf("calloc failed\n");
        return EXIT_FAILURE;
    }

    *vals = 5;
    printf("%d\n", vals[0]);

    void* tmp = realloc(vals, 8*sizeof(int));

    if (!tmp) {
        printf("realloc failed\n");
        free(tmp);
    }

    *(vals + 6) = 10;
    printf("%d\n", vals[6]);

    free(vals);

    return EXIT_SUCCESS;
}