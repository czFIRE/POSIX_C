#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    long input = 0;

    for (;;) {
        int return_val = scanf("%ld", &input);

        if (return_val == EOF) {
            putchar('\n');
            break;
        }

        if (return_val != 1) {
            fprintf(stderr, "Wrong args bruh\n");
            return 1;
        }

        for (size_t i = 0; i < 8; i++) {
            printf("%02lx ", input);
            input >>= 8;
        }

        putchar('\n');
    }

    return 0;
}
