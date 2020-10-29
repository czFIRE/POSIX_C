#include "utils.h"
#include "syslog_parser.h"

#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>

void read_real_functions(struct real_functions *functions)
{
    functions->real_calloc = dlsym(RTLD_NEXT, "calloc");
    functions->real_free = dlsym(RTLD_NEXT, "free");
    functions->real_malloc = dlsym(RTLD_NEXT, "malloc");
    functions->real_realloc = dlsym(RTLD_NEXT, "realloc");
    if (!functions->real_calloc || !functions->real_free ||
        !functions->real_malloc || !functions->real_realloc) {
        fprintf(stderr, "Error in dlsym: %s\n", dlerror());
        exit(EXIT_FAILURE);
    }
}

void initialise_service_block(struct service_info *info, size_t bytes)
{
    info->block_size = bytes;
    info->check = CANNARY_WORD;
}

int check_service_validity(struct service_info *info)
{
    return strcmp(info->check, CANNARY_WORD);
}

void print_statistics(struct memory_statistics *statistics)
{
    syslog(LOG_DEBUG, STATUS_INFO,
           statistics->total_used - statistics->total_freed,
           statistics->total_used, statistics->total_freed,
           statistics->allocation_num, statistics->dealllocation_num);
}

void read_enviroment_settings(struct enviroment_settings *settings)
{
    int memgrind_log_stderr = LOG_PID;
    char memgrind_log_level = LOG_DEBUG;

    /*settings->memgrind_alloc_count = -1;
    settings->memgrind_alloc_max = -1;
    settings->memgrind_mem_max = -1;*/

    char *env_var = getenv("MEMGRIND_LOG_STDERR");
    if (env_var != NULL && strcmp("", env_var) != 0) {
        memgrind_log_stderr = LOG_PERROR;
    }

    openlog("", memgrind_log_stderr, LOG_USER);

    env_var = getenv("MEMGRIND_LOG_LEVEL"); // if invalid, then return
    if (env_var != NULL) {
        memgrind_log_level = parse_log_value(env_var);
    }

    setlogmask(LOG_UPTO(memgrind_log_level));

    settings->memgrind_alloc_count = get_value("MEMGRIND_ALLOC_COUNT");
    settings->memgrind_alloc_max = get_value("MEMGRIND_ALLOC_MAX");
    settings->memgrind_mem_max = get_value("MEMGRIND_MEM_MAX");
}

size_t get_value(char *env_var_name)
{
    char *env_var = getenv(env_var_name);
    if (env_var == NULL) {
        return -1;
    }

    if (env_var[0] == '-') {
        syslog(LOG_ERR, "%s is negative", env_var_name);
        _Exit(EXIT_FAILURE);
    }

    char *endp;
    size_t value = strtol(env_var, &endp, 10);

    if (endp == env_var || *endp != 0) {
        syslog(LOG_ERR, "%s='%s' is invalid", env_var_name, env_var);
        _Exit(EXIT_FAILURE);
    }

    return value;
}