#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int *my_ptr = realloc(NULL, 4 * sizeof(int));
    if (!my_ptr) {
        printf("null realloc failed\n");
        return EXIT_FAILURE;
    }

    *my_ptr = 5;
    printf("after first alloc: %p\n", my_ptr);

    int *tmp = realloc(my_ptr, 4096 * sizeof(int));
    printf("%p\n", tmp);

    if (!tmp) {
        printf("realloc huge failed: %p\n", my_ptr);
        free(my_ptr); //this somehow shifted in a weird way
        return EXIT_FAILURE;
    }

    my_ptr = tmp;

    int *tmp1 = realloc(my_ptr, 1);
    printf("After shrink: %p\n", tmp);

    if (!tmp1) {
        printf("realloc shrink failed: %p\n", my_ptr);
        free(my_ptr);
        return EXIT_FAILURE;
    }

    my_ptr = tmp1;

    printf("Before last: %p\n", my_ptr);
    free(my_ptr);
    return EXIT_SUCCESS;
}