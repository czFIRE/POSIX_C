#define _GNU_SOURCE
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <getopt.h>
#include <unistd.h>

#include <syslog.h>

void help(const char *argv0)
{
    printf("Was too lazy to write help, sucks eh\n%s\n", argv0);
}

void version(void) { puts("log 1.0"); }

static const char optstring[] = "hvael:i:";
struct option longopts[] = {
    {.val = 'v', .name = "version"},
    {.val = 'h', .name = "help"},
    {.val = 'e', .name = "stderr"},
    {.val = 'l', .name = "level", .has_arg = required_argument},
    {.val = 'i', .name = "id", .has_arg = required_argument},
    {0},
};

struct options {
    int level;
    char *id;
};

int get_log_level(char *level);

int main(int argc, char *argv[], char *env[])
{
    int opt;

    // initialize options
    struct options options = {LOG_INFO, NULL};

    int open_log_opt = LOG_CONS;

    // call getopt() until there are no options left
    while ((opt = getopt_long(argc, argv, optstring, longopts, NULL)) != -1) {
        switch (opt) {
        case 'h':
            help(argv[0]);
            return EXIT_SUCCESS;

        case 'v':
            version();
            return EXIT_SUCCESS;

        case 'e':
            open_log_opt = LOG_PERROR;
            break;
        case 'l':
            // should do a nice parser here
            options.level = get_log_level(optarg);
            if (options.level == -1) {
                fprintf(stderr, "Wrong level argument");
                return EXIT_FAILURE;
            }
            break;
        case 'i':
            options.id = optarg;
            break;

        default:
            help(argv[0]);
            return EXIT_FAILURE;
        }
    }

    // zkontroluj, zda mi tam vubec nejakou zpravu dal

    openlog(options.id, open_log_opt, LOG_USER);

    syslog(options.level, "%s\n", argv[optind]);

    closelog();

    return EXIT_SUCCESS;
}

int get_log_level(char *level)
{
    int result = -1;
    if (strcmp(level, "LOG_EMERG") == 0) result = LOG_EMERG;
    if (strcmp(level, "LOG_ALERT") == 0) result = LOG_ALERT;
    if (strcmp(level, "LOG_CRIT") == 0) result = LOG_CRIT;
    if (strcmp(level, "LOG_ERR") == 0) result = LOG_ERR;
    if (strcmp(level, "LOG_WARNING") == 0) result = LOG_WARNING;
    if (strcmp(level, "LOG_NOTICE") == 0) result = LOG_NOTICE;
    if (strcmp(level, "LOG_INFO") == 0) result = LOG_INFO;
    if (strcmp(level, "LOG_DEBUG") == 0) result = LOG_DEBUG;

    return result;
}