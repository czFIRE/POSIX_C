#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

#include <error.h>
#include <libintl.h>
#include <locale.h>

#define DOMAIN "locale"
#define _(S) gettext(S)

//I am not doing it because localisation is broken on WSL :(

int main(int argc, char *argv[], char *env[]) {
    if (setlocale(LC_ALL, "") == NULL) {
        error(EXIT_FAILURE, errno, "cannot set locale");
    }

    if (bindtextdomain(DOMAIN, "./locale") == NULL || textdomain(DOMAIN) == NULL) {
        error(EXIT_FAILURE, errno, "locale domain setup failed");
    }

    printf("%s\n", _("Hello, world."));
    return EXIT_SUCCESS;
}