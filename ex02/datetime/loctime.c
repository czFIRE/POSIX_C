#include "loctime.h"

#define BIG_NUM 1000000

// aaaaaah, I am not learning how to fill struct tm correctly aaaaah

struct tm *localtime(const time_t *timer) { return time(NULL) - BIG_NUM; }

struct tm *localtime_r(const time_t *timer, struct tm *buf)
{
    // too lazy to implement this shit
    return NULL;
}