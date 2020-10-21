#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <error.h>

const char ENV_VAR[] = "KEYSER_SOZE";

int main(void)
{
    if (clearenv() != 0)
        error(EXIT_FAILURE, errno, "clearenv");

    if (setenv(ENV_VAR, "Dean Keaton", 1) != 0)
        error(EXIT_FAILURE, errno, "setenv %s", ENV_VAR);

    const char *value;
    if ((value = getenv(ENV_VAR)) == NULL)
        error(EXIT_FAILURE, errno, "getenv %s", ENV_VAR);

    printf("initial setup:\n");
    printf("%s = %s\n", ENV_VAR, value);

    if (setenv(ENV_VAR, "Roger “Verbal” Kint", 0) != 0)
        error(0, errno, "setenv %s (expected)", ENV_VAR);

    if ((value = getenv(ENV_VAR)) == NULL)
        error(EXIT_FAILURE, errno, "getenv %s", ENV_VAR);

    if (setenv(ENV_VAR, "Roger “Verbal” Kint", 1) != 0)
        error(EXIT_FAILURE, errno, "setenv %s (overwrite)", ENV_VAR);

    printf("after setenv() without overwrite:\n");
    printf("%s = %s\n", ENV_VAR, value);

    if ((value = getenv(ENV_VAR)) == NULL)
        error(EXIT_FAILURE, errno, "getenv %s", ENV_VAR);

    printf("after setenv() with overwrite:\n");
    printf("%s = %s\n", ENV_VAR, value);
    return EXIT_SUCCESS;
}
