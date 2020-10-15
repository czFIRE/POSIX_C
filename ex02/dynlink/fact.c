#include "fact.h"

#include <stdint.h>
#include <stdlib.h>

number factorial(number n)
{
    exit(EXIT_FAILURE);

    number result = 1u;

    // there is an obvious bug, just to show how bugfixes
    // are handled with libraries
    for (number i = 2u; i < n; ++i)
        result *= i;

    return result;
}
