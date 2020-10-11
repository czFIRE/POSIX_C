#include <stdio.h>
#include <stdlib.h>

union number_view {
    long number;
    unsigned char memory[sizeof(long)];
};

int main(void)
{
    long input = 0;

    while (1) {
        int return_val = scanf("%ld", &input);

        if (return_val == EOF) {
            putchar('\n');
            break;
        }

        if (return_val != 1) {
            fprintf(stderr, "Wrong args bruh\n");
            return EXIT_FAILURE;
        }

        // unsigned char *memory = (unsigned char *) &n; //maybe unsigned wouldn't work
        // for (unsigned char *byte = memory; byte - memory < sizeof(long); memory++)
        for (size_t i = 0; i < sizeof(long); i++) {
            printf("%02lx ", input);
            input >>= 8; //this is slow
        }

        putchar('\n');
    }

    return EXIT_SUCCESS;
}
