#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int *my_ptr = realloc(NULL, 4 * sizeof(int));
    *my_ptr = 5;
    printf("%p\n", my_ptr);

    int *tmp = realloc(my_ptr, 4096 * sizeof(int));
    printf("%p\n", tmp);

    if (!tmp) {
        printf("realloc failed\n");
        free(my_ptr);
    }

    my_ptr = tmp;

    int *tmp1 = realloc(my_ptr, 1);
    printf("%p\n", tmp);

    if (!tmp1) {
        printf("realloc failed\n");
        free(my_ptr);
    }

    my_ptr = tmp1;

    free(my_ptr);
    return EXIT_SUCCESS;
}