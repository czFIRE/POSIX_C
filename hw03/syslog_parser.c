#include "syslog_parser.h"

#include <string.h>
#include <syslog.h>

struct syslog_priority {
    const char *name;
    int priority;
};

struct syslog_priority syslog_priority_map[] = {
    {"EMERG", LOG_EMERG}, {"ALERT", LOG_ALERT},     {"CRIT", LOG_CRIT},
    {"ERR", LOG_ERR},     {"WARNING", LOG_WARNING}, {"NOTICE", LOG_NOTICE},
    {"INFO", LOG_INFO},   {"DEBUG", LOG_DEBUG},     {0},
};

#define LOG_MAX_STATUS (LOG_DEBUG + 1)

int parse_log_value(char *log_val)
{
    for (size_t i = 0; i < LOG_MAX_STATUS; i++) {
        if (strcmp(log_val, syslog_priority_map[i].name) == 0) {
            return syslog_priority_map[i].priority;
        }
    }

    return -1;
}