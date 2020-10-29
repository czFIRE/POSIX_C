#include "syslog_parser.h"

#include <string.h>

int parse_log_value(char *log_val)
{
    for (size_t i = 0; i < LOG_MAX_STATUS; i++) {
        if (strcmp(log_val, syslog_priority_map[i].name) == 0) {
            return syslog_priority_map[i].priority;
        }
    }

    return -1;
}