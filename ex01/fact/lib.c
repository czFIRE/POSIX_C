#include "lib.h"

int fact(int number)
{
    if (number <= 1) {
        return 1;
    }
    return number * fact(number - 1);
}