#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void print_argv(int argc, char *argv[])
{
    for (size_t i = 1; i < argc; i++) {
        printf("%s ", argv[i]);
    }
    putchar('\n');
}

int compare(const void *p1, const void *p2) { 
    //
    return strcoll(p2, p1); 
}

int main(int argc, char *argv[], char *env[])
{
    //char *test[2] = {"b", "a"};
    //qsort(test, 2, sizeof(char*), compare);
    //printf("%s %s\n", test[0], test[1]);
    
    print_argv(argc, argv);
    qsort(argv + 1, argc - 1, sizeof(char *), compare);
    print_argv(argc, argv);

    return EXIT_SUCCESS;
}