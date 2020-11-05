#define _GNU_SOURCE

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <error.h>
#include <getopt.h>
#include <libgen.h>
#include <unistd.h>

int _logexpr(const char *expr_str, int expr_val, bool is_errcode)
{
    printf("\x1b[97m * \x1b[0m\x1b[92m%s\x1b[0m", expr_str);

    if (is_errcode) {
        printf(" (%s%d %s%s)\n",
                expr_val == 0 ? "\x1b[92m" : "\x1b[91m",
                expr_val,
                expr_val == 0 ? "success" : "error",
                "\x1b[0m"
        );
    } else {
        printf(" = \x1b[97m%d\x1b[0m\n", expr_val);
    }

    return expr_val;
}

#define LOGEXPR(EXPR) \
    _logexpr(#EXPR, (EXPR), false)

#define LOGCALL(EXPR) \
    _logexpr(#EXPR, (EXPR), true)

#define LOGNOTIFY(FORMAT, ...) \
    printf("\x1b[96m * \e[0m\x1b[96m" FORMAT "\x1b[0m\n", ## __VA_ARGS__)

#define LOGINFO(FORMAT, ...) \
    printf("\x1b[94m * \x1b[0m" FORMAT "\n", ## __VA_ARGS__)

#define LOGSUCCESS(FORMAT, ...) \
    printf("\x1b[92m * " FORMAT "\n", ## __VA_ARGS__)

#define LOGWARN(ECODE, FORMAT, ...) \
    error(0, ECODE, "\x1b[93m" FORMAT "\x1b[0m", ## __VA_ARGS__)

#define LOGDIE(ECODE, FORMAT, ...) \
    error(EXIT_FAILURE, ECODE, "\x1b[91m" FORMAT "\x1b[0m", ## __VA_ARGS__)

static
void check_privileges(void)
{
    uid_t ruid, euid, suid;
    if (getresuid(&ruid, &euid, &suid) == -1)
        LOGDIE(errno, "getresuid");

    if (ruid == euid)
        LOGDIE(0, "program is not set-uid");
}

static
void print_privileges(void)
{
    uid_t ruid, euid, suid;
    if (getresuid(&ruid, &euid, &suid) == -1)
        LOGDIE(errno, "getresuid");

    LOGINFO("(RUID %4d)  (EUID %4d)  (SUID %4d)", ruid, euid, suid);
}

static
void drop_privileges(bool correct)
{
    LOGNOTIFY("program started");
    print_privileges();

    puts("");

    LOGNOTIFY("dropping privileges temporarily");
    uid_t old_euid;
    LOGEXPR(old_euid = geteuid());

    if (LOGCALL(seteuid(getuid())) != 0)
        LOGWARN(errno, "seteuid");

    print_privileges();

    puts("");
    if (!correct) {
        LOGNOTIFY("dropping privileges premanently (\x1b[91mthe wrong way\x1b[0m)");

        if (LOGCALL(setuid(getuid())) != 0)
            LOGWARN(errno, "setuid");

        print_privileges();
    } else {
        LOGNOTIFY("dropping privileges permanently (\x1b[92mthe right way\x1b[0m)");

        if (LOGCALL(seteuid(old_euid)) != 0)
            LOGWARN(errno, "setuid");

        print_privileges();

        if (LOGCALL(setuid(getuid())) != 0)
            LOGWARN(errno, "setuid");

        print_privileges();
    }

    puts("");
    LOGNOTIFY("check that we dropped priviliges correctly");

    if (LOGCALL(setuid(old_euid)) != -1)
        LOGDIE(0, "old priviliges restored, potential security problem detected");

    puts("");
    LOGSUCCESS("privileges dropped correctly");
}

//-----------------------------------------------------------------------------

void help(void)
{
    printf(
        "NAME\n"
        "    drop -- demonstrate privileges drop\n"
        "\n"
        "SYNOPSIS\n"
        "    drop -h\n"
        "    drop (-w|-c)\n"
        "\n"
        "OPTIONS\n"
        "    -h     show help and exit\n"
        "\n"
        "    -w     drop permissions, the wrong way\n"
        "    -c     drop permissions, the correct way\n"
    );
}

void usage(char *argv0)
{
    printf("usage: %s [-h] (-w|-c)\n", basename(argv0));
}


static
const char *optstring = "hcw";

int main(int argc, char *argv[])
{
    int commands = 0;
    bool correct = false;

    int opt;
    while ((opt = getopt(argc, argv, optstring)) != -1) {
        switch (opt) {
        case 'h':
            help();
            return EXIT_SUCCESS;

        case 'c':
            ++commands;
            correct = true;
            break;

        case 'w':
            ++commands;
            break;

        default:
            return EXIT_FAILURE;
        }
    }

    if (commands != 1 || optind != argc) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    check_privileges();
    drop_privileges(correct);

    return EXIT_SUCCESS;
}
