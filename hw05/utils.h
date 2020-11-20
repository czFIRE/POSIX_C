#ifndef ID_UTILS_H
#define ID_UTILS_H

#include <unistd.h>

struct name_id {
    // I can leave here gid_t as both uid_t and gid_t are typedeffed unsigned
    // ints
    gid_t real;
    gid_t effective;
    char *name;
};

struct id_data {
    struct name_id *user_data;
    struct name_id *group_data;
    struct name_id **groups_data;
};

void get_process_info(void);
void get_user_info(char *user_name);

void version(void);
void help(void);

void print_data(struct id_data data, size_t length);

#endif