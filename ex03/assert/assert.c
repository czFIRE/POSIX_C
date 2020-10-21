#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    int i = 2;
    int j = 0;

    assert(j != 0);

    printf("%d / %d = %d\n", i, j, i / j);
    return EXIT_SUCCESS;
}
