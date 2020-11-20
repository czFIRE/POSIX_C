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
void proper_print(gid_t effective, gid_t real, char *name,
                  struct program_options options);

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

void get_process_info(struct program_options options)
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

    size_t ngroups = getgroups(0, NULL);
    gid_t *groups_ids = calloc(ngroups + 1, sizeof(gid_t));
    if (groups_ids == NULL) {
        free(group_data.name);
        fprintf(stderr, "groups_ids malloc failed");
        exit(EXIT_FAILURE);
    }

    if (getgroups(ngroups, groups_ids) == -1) {
        free(groups_ids);
        free(group_data.name);
        error(EXIT_FAILURE, errno, "getgroups");
    }

    struct name_id *groups_data = calloc(ngroups + 1, sizeof(struct name_id));
    if (groups_data == NULL) {
        free(groups_ids);
        free(group_data.name);
        fprintf(stderr, "groups_data malloc failed");
        exit(EXIT_FAILURE);
    }

    data_id.groups_data = &groups_data;

    // check if the process is in this
    for (size_t i = 0; i < ngroups; i++) {
        struct group *gsinfo = getgrgid(groups_ids[i]);
        if (gsinfo == NULL) {
            free_memory(data_id, ngroups);
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
    print_data(data_id, ngroups, options);
    free_memory(data_id, ngroups);
}

void get_user_info(char *user_name, struct program_options options)
{
    // getpwnam / getpwuid (if I get UID as input)
    struct passwd *uinfo;

    char *endp;
    int uid = strtol(user_name, &endp, 10);

    if (user_name == endp) {
        uinfo = getpwnam(user_name);
        if (uinfo == NULL) {
            fprintf(stderr, "getpwnam failed\n");
            // this can be here because this will occur only with internal
            // failure
            exit(EXIT_FAILURE);
        }
    } else if (*endp == '\0') {
        uinfo = getpwuid(uid);
        if (uinfo == NULL) {
            fprintf(stderr, "getpwuid failed\n");
            // this can be here because this will occur only with internal
            // failure
            exit(EXIT_FAILURE);
        }
    } else {
        fprintf(stderr, "Invalid user name / user id\n");
        exit(EXIT_FAILURE);
    }

    struct name_id user_data = {uinfo->pw_uid, uinfo->pw_uid, uinfo->pw_name};

    struct id_data data_id = {0};
    data_id.user_data = &user_data;

    // getgrgid

    struct group *ginfo = getgrgid(uinfo->pw_gid);
    if (ginfo == NULL) {
        fprintf(stderr, "getgrgid failed\n");
        // this can be here because this will occur only with internal failure
        exit(EXIT_FAILURE);
    }

    struct name_id group_data = {ginfo->gr_gid, ginfo->gr_gid,
                                 strdup(ginfo->gr_name)};

    data_id.group_data = &group_data;

    // get group list

    int ngroups = 0;
    if (getgrouplist(uinfo->pw_name, uinfo->pw_gid, NULL, &ngroups) == -1) {
        // maybe I could merge these 2
        if (ngroups <= 0) {
            free(group_data.name);
            fprintf(stderr, "can't get ngroups");
            exit(EXIT_FAILURE);
        }
    }

    gid_t *groups_ids = calloc(ngroups + 1, sizeof(gid_t));
    if (groups_ids == NULL) {
        free(group_data.name);
        fprintf(stderr, "groups_ids malloc failed");
        exit(EXIT_FAILURE);
    }

    if (getgrouplist(uinfo->pw_name, uinfo->pw_gid, groups_ids, &ngroups) ==
        -1) {
        free(group_data.name);
        free(groups_ids);
        error(EXIT_FAILURE, 0, "%s: Group list changed,", uinfo->pw_name);
    }

    struct name_id *groups_data = calloc(ngroups + 1, sizeof(struct name_id));
    if (groups_data == NULL) {
        free(groups_ids);
        free(group_data.name);
        fprintf(stderr, "groups_data malloc failed");
        exit(EXIT_FAILURE);
    }

    data_id.groups_data = &groups_data;

    for (int i = 0; i < ngroups; i++) {
        struct group *gsinfo = getgrgid(groups_ids[i]);
        if (gsinfo == NULL) {
            free_memory(data_id, ngroups);
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
    print_data(data_id, ngroups, options);
    free_memory(data_id, ngroups);
}

void print_data(struct id_data data, size_t length,
                struct program_options options)
{
    bool print_all = ((options.group + options.groups + options.user) == 0);
    if (print_all || options.user) {
        if (print_all)
            printf("%s=%d(%s) ", "uid", data.user_data->effective,
                   data.user_data->name);
        else {
            proper_print(data.user_data->effective, data.user_data->real,
                         data.user_data->name, options);
        }
    }

    if (print_all || options.group) {
        if (print_all)
            printf("%s=%d(%s) ", "gid", data.user_data->effective,
                   data.user_data->name);
        else {
            proper_print(data.group_data->effective, data.group_data->real,
                         data.group_data->name, options);
        }
    }

    if (print_all || options.groups) {
        if (print_all)
            printf("groups=");

        for (size_t i = 0; i < length - 1; i++) {
            if (print_all) {
                printf("%d(%s), ", (*data.groups_data + i)->effective,
                       (*data.groups_data + i)->name);
            } else {
                proper_print((*data.groups_data + i)->effective,
                             (*data.groups_data + i)->real,
                             (*data.groups_data + i)->name, options);
                putchar(' ');
            }
        }

        if (print_all) {
            printf("%d(%s)", (*data.groups_data + length - 1)->effective,
                   (*data.groups_data + length - 1)->name);
        } else {
            proper_print((*data.groups_data + length - 1)->effective,
                         (*data.groups_data + length - 1)->real,
                         (*data.groups_data + length - 1)->name, options);
        }
    }

    putchar('\n');
}

void proper_print(gid_t effective, gid_t real, char *name,
                  struct program_options options)
{
    if (options.name) {
        printf("%s", name);
    } else {
        if (options.real) {
            printf("%d", real);
        } else {
            printf("%d", effective);
        }
    }
}

void free_memory(struct id_data data, size_t length)
{
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