#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include <error.h>

const char ENV_VAR[] = "KEYSER_SOZE";

int main(void)
{
    if (clearenv() != 0)
        error(EXIT_FAILURE, errno, "clearenv");

    char *env_entry = malloc(1024 * sizeof(char));
    if (env_entry == NULL)
        error(EXIT_FAILURE, errno, "cannot allocate memory");

    snprintf(env_entry, 1024, "%s=%s", ENV_VAR, "Dean Keaton");
    if (putenv(env_entry) != 0)
        error(EXIT_FAILURE, errno, "putenv");

    const char *value;
    if ((value = getenv(ENV_VAR)) == NULL)
        error(EXIT_FAILURE, errno, "getenv %s", ENV_VAR);

    printf("after initial setup:\n");
    printf("%s = %s\n", ENV_VAR, value);

    snprintf(env_entry, 1024, "%s=%s", ENV_VAR, "Roger “Verbal” Kint");
//  if (putenv(env_entry) != 0)
//      error(EXIT_FAILURE, errno, "putenv");

    if ((value = getenv(ENV_VAR)) == NULL)
        error(EXIT_FAILURE, errno, "getenv %s", ENV_VAR);

    printf("after modifying the string in memory:\n");
    printf("%s = %s\n", ENV_VAR, value);
    return EXIT_SUCCESS;
}
