#ifndef LOCTIME_H
#define LOCTIME_H

#include <time.h>

struct tm *localtime(const time_t * timer);
struct tm *localtime_r(const time_t * timer, struct tm *buf);

#endif