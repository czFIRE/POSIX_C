#ifndef SYSLOG_PARSER
#define SYSLOG_PARSER

#include <syslog.h>

struct syslog_priority {
    const char *name;
    int priority;
};

struct syslog_priority syslog_priority_map[] = {
    {"EMERG", LOG_EMERG},
    {"ALERT", LOG_ALERT},
    {"CRIT", LOG_CRIT},
    {"ERR", LOG_ERR},
    {"WARNING", LOG_WARNING},
    {"NOTICE", LOG_NOTICE},
    {"INFO", LOG_INFO},
    {"DEBUG", LOG_DEBUG},
    {0},
};

#define LOG_MAX_STATUS (LOG_DEBUG + 1)

int parse_log_value(char *log_val);

#endif // SYSLOG_PARSER