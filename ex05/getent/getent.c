#define _GNU_SOURCE

#include <errno.h>
#include <error.h>
#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <grp.h>
#include <pwd.h>
#include <sys/types.h>

const char *optstring = "ugm";

void print_users(bool highlight);
void print_groups(bool highlight);

int main(int argc, char *argv[])
{
    bool users = false;
    bool groups = false;
    bool highlight = false;

    int opt;
    while ((opt = getopt(argc, argv, optstring)) != -1) {
        switch (opt) {
        // maybe add some case for help?
        case 'u':
            users = true;
            break;

        case 'g':
            groups = true;
            break;

        case 'm':
            highlight = true;
            break;

        default:
            return EXIT_FAILURE;
        }
    }

    if (!users && !groups) {
        fprintf(stderr, "No modes set.\n");
        return EXIT_FAILURE;
    }

    if (users) {
        print_users(highlight);
    }

    if (groups) {
        print_groups(highlight);
    }

    return EXIT_SUCCESS;
}

void print_users(bool highlight)
{
    uid_t ruid, euid, suid;
    if (getresuid(&ruid, &euid, &suid) == -1)
        error(0, errno, "getresuid");

    // printf("(RUID %4d)  (EUID %4d)  (SUID %4d)\n", ruid, euid, suid);

    printf("Users:");
    struct passwd *entry;

    while ((entry = getpwent()) != NULL) {
        if (highlight && entry->pw_uid == ruid) {
            putchar('*');
        }
        printf("\t%4d %s\n", entry->pw_uid, entry->pw_name);
    }

    putchar('\n');
}

void print_groups(bool highlight)
{
    uid_t rgid, egid, sgid;
    if (getresgid(&rgid, &egid, &sgid) == -1)
        error(0, errno, "getresgid");

    printf("Groups:");
    struct group *entry;

    // getgroups :O -> tohle je pro suplementární skupiny procesu
    // wtf was I even doing
    while ((entry = getgrent()) != NULL) {
        if (highlight && entry->gr_gid == rgid) {
            putchar('*');
        }
        printf("\t%4d %s\n", entry->gr_gid, entry->gr_name);
    }

    putchar('\n');
}

//gegrouplist je všechno pro uživatele