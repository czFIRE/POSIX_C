#include <dlfcn.h>
#include <err.h>
#include <stdio.h>
#include <stdlib.h>

typedef double(*f_dbl)(double);

void help(const char *argv0)
{
    printf(
        "usage: %s LIBRARY [FUNCTION]\n"
        "\n"
        "Opens the given math library and symbol (log10 by default)\n"
        "then asks for a number and returns the output.\n"
        "\n"
        "You can locate math libraries on your system by running\n"
        "    locate libm.so\n"
        , argv0);
}

int main(int argc, char *argv[])
{
    /* show help if there are no parameters */
    if (argc < 2 || argc > 3) {
        help(argv[0]);
        return EXIT_FAILURE;
    }

    /* we only set this to EXIT_SUCCESS if everything goes right */
    int status = EXIT_FAILURE;

    char *libname = argv[1];
    char *symbol  = argv[2] ? argv[2] : "log10";

    /* let's open the library */
    void *libhndl = dlopen(libname, RTLD_NOW);

    /* dlopen returns NULL on failure */
    if (!libhndl) {
        warnx("dlopen: %s", dlerror());
        goto main_dlopen_error;
    }

    /* get function (symbol) from the library */
    f_dbl f = (f_dbl) dlsym(libhndl, symbol);

    /* dlsym returns NULL on failure */
    if (!f) {
        warnx("dlsym: %s", dlerror());
        goto main_dlsym_error;
    }

    /* ok, let's read input from the user */
    printf("%s> ", symbol);
    double input;
    if (scanf("%lf", &input) != 1) {
        fprintf(stderr, "invalid input\n");
        goto main_dlsym_error;
    }

    printf("%s(%lf) = %lf\n", symbol, input, f(input));
    status = EXIT_SUCCESS;

main_dlsym_error:
    dlclose(libhndl);
main_dlopen_error:
    return status;
}
