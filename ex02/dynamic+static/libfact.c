//#include "libfact.h"

#include "num.h"

int fact(int number)
{
    if (number <= 1) {
        return 1;
    }
    return number * fact(number - 1);
}