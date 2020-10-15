//#include "libcomb.h"
//#include "libfact.h"
#include "num.h"

int comb(int upper, int lower)
{
    // this is done so ineffectively that I wanna cry, but am lazy to implement
    // it otherwise
    return fact(upper) / (fact(upper - lower) * fact(lower));
}