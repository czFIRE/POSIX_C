#define _GNU_SOURCE
#include "utils.h"
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <error.h>
#include <getopt.h>
#include <libgen.h>
#include <unistd.h>

#include <sys/types.h>
#include <pwd.h>

void help(void)
{
    printf(
"Usage: id [OPTION] [USER...]\n"
"Print user and group information for the specified USER,\n"
"or (when USER omitted) for the current user.\n"
"\n"
"  -g, --group    print only the effective group ID\n"
"  -G, --groups   print all group IDs\n"
"  -n, --name     print a name instead of a number, for -ugG\n"
"  -r, --real     print the real ID instead of the effective ID, with -ugG\n"
"  -u, --user     print only the effective user ID\n"
"      --help     display this help and exit\n"
"      --version  output version information and exit\n"
"\n"
"Without any OPTION, print some useful set of identified information.\n");
}

void version(void) { puts("id 1.0"); }

void get_process_info(void) {
    //get user ids
    uid_t ruid, euid, suid;
    if (getresuid(&ruid, &euid, &suid) == -1) {
        error(EXIT_FAILURE, errno, "getresuid");
    }

    //get user name -> doopravdy to žere efektivní?
    //getpwuid(euid)

    //get group ids
    //getresgid

    //get group name
    //getgrgid(egid)


    //get groups 
    //getgroups / getgrouplist
}

void get_user_info(char * user_name) {

}