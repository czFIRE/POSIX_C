#define _GNU_SOURCE
#include "utils.h"
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <error.h>
#include <getopt.h>
#include <libgen.h>
#include <unistd.h>

#include <grp.h>
#include <pwd.h>
#include <sys/types.h>

void free_memory(struct id_data data, size_t length);

void help(void)
{
    printf("Usage: id [OPTION] [USER...]\n"
           "Print user and group information for the specified USER,\n"
           "or (when USER omitted) for the current user.\n"
           "\n"
           "  -g, --group    print only the effective group ID\n"
           "  -G, --groups   print all group IDs\n"
           "  -n, --name     print a name instead of a number, for -ugG\n"
           "  -r, --real     print the real ID instead of the effective ID, "
           "with -ugG\n"
           "  -u, --user     print only the effective user ID\n"
           "      --help     display this help and exit\n"
           "      --version  output version information and exit\n"
           "\n"
           "Without any OPTION, print some useful set of identified "
           "information.\n");
}

void version(void) { puts("id 1.0"); }

void get_process_info(void)
{
    // get user ids
    uid_t ruid, euid, suid;
    if (getresuid(&ruid, &euid, &suid) == -1) {
        error(EXIT_FAILURE, errno, "getresuid");
    }

    // getpwuid(euid)

    struct passwd *uinfo = getpwuid(euid);
    if (uinfo == NULL) {
        fprintf(stderr, "getpwuid failed");
        // this can be here because this will occur only with internal failure
        exit(EXIT_FAILURE);
    }

    struct name_id user_data = {euid, ruid, uinfo->pw_name};

    struct id_data data_id = {0};
    data_id.user_data = &user_data;

    // get group ids
    // getresgid

    gid_t rgid, egid, sgid;
    if (getresuid(&rgid, &egid, &sgid) == -1) {
        error(EXIT_FAILURE, errno, "getresuid");
    }

    // get group name
    // getgrgid(egid)

    struct group *ginfo = getgrgid(egid);
    if (ginfo == NULL) {
        fprintf(stderr, "getgrgid failed");
        // this can be here because this will occur only with internal failure
        exit(EXIT_FAILURE);
    }

    struct name_id group_data = {egid, rgid, strdup(ginfo->gr_name)};
    if (group_data.name == NULL) {
        fprintf(stderr, "group data malloc failed");
        exit(EXIT_FAILURE);
    }
    
    data_id.group_data = &group_data;
    // get groups
    // getgroups / getgrouplist

    // this will probably be it's own function

    size_t num_groups = getgroups(0, NULL);
    gid_t *groups_ids = calloc(num_groups + 1, sizeof(gid_t));
    if (groups_ids == NULL) {
        fprintf(stderr, "groups_ids malloc failed");
        exit(EXIT_FAILURE);
    }

    if (getgroups(num_groups, groups_ids) == -1) {
        free(groups_ids);
        free(group_data.name);
        error(EXIT_FAILURE, errno, "getgroups");
    }

    struct name_id *groups_data = calloc(num_groups + 1, sizeof(struct name_id));

    data_id.groups_data = &groups_data;

    // check if the process is in this
    for (size_t i = 0; i < num_groups; i++) {
        struct group *gsinfo = getgrgid(groups_ids[i]);
        if (gsinfo == NULL) {
            free_memory(data_id, num_groups);
            free(groups_ids);
            fprintf(stderr, "getgrgid failed");
            // this can be here because this will occur only with internal
            // failure
            exit(EXIT_FAILURE);
        }

        groups_data[i].name = strdup(gsinfo->gr_name);
        groups_data[i].effective = gsinfo->gr_gid;
        // can here be a real / effective?
        groups_data[i].real = groups_data[i].effective;
    }
    free(groups_ids);
    print_data(data_id);
    free_memory(data_id, num_groups);
}

void get_user_info(char *user_name)
{
    // getpwnam / getpwuid (if I get UID as input)

    // getgrgid
}

void print_data(struct id_data data)
{
    printf("uid=%d(%s) ", data.user_data->effective, data.user_data->name);
    printf("gid=%d(%s) ", data.group_data->effective, data.group_data->name);
    printf("groups=");

    // here can be a hillarious bug with memory access
    /*for (size_t i = 0; i < length - 1; i++) {
        printf("%d(%s), ", (*data.groups_data + i)->effective,
               (*data.groups_data + i)->name);
    }*/

    size_t iter = 0;
    while ((*data.groups_data + iter + 1)->name != 0) {
        printf("%d(%s), ", (*data.groups_data + iter)->effective,
               (*data.groups_data + iter)->name);
        iter++;
    }

    printf("%d(%s)\n", (*data.groups_data + iter)->effective,
           (*data.groups_data + iter)->name);
}

void free_memory(struct id_data data, size_t length) {
    if (data.group_data) {
        free(data.group_data->name);
    }

    if (data.groups_data) {
        for (size_t i = 0; i < length; i++) {
            free((*data.groups_data + i)->name);
        }
        free(*data.groups_data);
    }
}