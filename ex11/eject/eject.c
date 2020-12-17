#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <error.h>
#include <fcntl.h>
#include <getopt.h>
#include <linux/cdrom.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define BUG(FMT, ...)                                                   \
    do {                                                                \
        error(0, 0, "%s: BUG: " FMT, __func__, ## __VA_ARGS__);         \
        abort();                                                        \
    } while (0)

static const
char default_device[] = "/dev/cdrom";

// actions
typedef enum {
    ACTION_CLOSE,
    ACTION_OPEN,
    ACTION_TOGGLE
} action;

// simple help
void help(char *argv0)
{
    printf(
        "USAGE\n"
        "    %s [OPTIONS] [DEVICE]\n"
        "    default device: '%s'\n"
        "\n"
        "OPTIONS\n"
        "    -t     close tray\n"
        "    -T     toggle tray state\n"
        "    -h     show this help\n",
        argv0, default_device);
}

// applies the operation 'act' on device specified by 'file'
void eject(const char *file, action act)
{
    int fd = open(file, O_RDWR | O_NONBLOCK);

    if (fd == -1)
        error(EXIT_FAILURE, errno, "%s", file);

    // get CDROM status
    int status = ioctl(fd, CDROM_DRIVE_STATUS, 0);

    if (status == -1)
        error(EXIT_FAILURE, errno, "%s: CDROM_DRIVE_STATUS", file);

    bool is_closed  = !(status & CDS_TRAY_OPEN);

    // get capabilities
    int capability = ioctl(fd, CDROM_GET_CAPABILITY, 0);

    if (capability == -1)
        error(EXIT_FAILURE, errno, "%s: CDROM_GET_CAPABILITY", file);

    if (act == ACTION_TOGGLE)
        act = is_closed ? ACTION_OPEN : ACTION_CLOSE;

    // now, try to open or close the tray
    switch (act) {
    case ACTION_OPEN:
        // first test that the drive can even do that
        if (!(capability & CDC_OPEN_TRAY))
            error(EXIT_FAILURE, 0, "%s: missing CDC_OPEN_TRAY", file);

        // unlock door
        if (ioctl(fd, CDROM_LOCKDOOR, 0) == -1)
            error(EXIT_FAILURE, errno, "%s: CDROM_LOCKDOOR", file);

        // eject the tray
        if (ioctl(fd, CDROMEJECT, 0) == -1)
            error(EXIT_FAILURE, errno, "%s: CDROMEJECT", file);
        break;

    case ACTION_CLOSE:
        // first test that the drive can do that
        if (!(capability & CDC_CLOSE_TRAY))
            error(EXIT_FAILURE, errno, "%s: missing CDC_CLOSE_TRAY", file);

        // close tray
        if (ioctl(fd, CDROMCLOSETRAY, 0) == -1)
            error(EXIT_FAILURE, errno, "%s: CDROMCLOSETRAY", file);
        break;

    default:
        BUG("value of act = %d not expected", act);
    }

    // done, perhaps
    close(fd);
}

int main(int argc, char *argv[])
{
    int    opt;
    action act = ACTION_OPEN;

    while ((opt = getopt(argc, argv, "tTh")) != -1) {
        switch (opt) {
        case 't':
            act = ACTION_CLOSE;
            break;
        case 'T':
            act = ACTION_TOGGLE;
            break;

        case 'h':
            help(argv[0]);
            return EXIT_SUCCESS;
        default:
            fprintf(stderr, "try '%s -h' for help\n", argv[0]);
            return EXIT_FAILURE;
        }
    }

    if (argc - optind > 1) {
        fprintf(stderr, "too many devices, try '%s -h' for help\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *device = (argc - optind == 1) ? argv[optind] : default_device;

    eject(device, act);
    return EXIT_SUCCESS;
}
