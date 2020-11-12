#define _GNU_SOURCE

#define CONFIG_USE_PTRACE_SYSCALL_INFO 0
#ifndef CONFIG_USE_PTRACE_SYSCALL_INFO
#  define CONFIG_USE_PTRACE_SYSCALL_INFO (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 3, 0))
#endif

#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <error.h>
#include <linux/version.h>
#include <sys/prctl.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <unistd.h>

#if CONFIG_USE_PTRACE_SYSCALL_INFO
#  include <linux/ptrace.h>
#else
#  include <sys/ptrace.h>
#endif

void usage(const char *argv0)
{
    fprintf(stderr, "usage: %s PROGRAM [ARGS...]\n", basename(argv0));
}

//--  child process  ----------------------------------------------------------

void run_tracee(char *path, char *argv[])
{
    // tell kernel that we want to be traced by our parent
    if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) == -1)
        error(EXIT_FAILURE, errno, "%d: failed to set up tracing", getpid());

    // stop so that our parent can set up his ptrace and catch up with us
    if (raise(SIGSTOP) != 0)
        error(EXIT_FAILURE, errno, "%d: failed to stop", getpid());

    // execute the target program
    execvp(path, argv);
    error(EXIT_FAILURE, errno, "%d: failed to exec '%s'", getpid(), argv[1]);
}

//--  parent process  ---------------------------------------------------------

size_t count_syscalls(pid_t child)
{
    size_t counter = 0;
    int wstatus;

    while (1) {
        if (waitpid(child, &wstatus, 0) == -1) {
            error(0, errno, "%d: waitpid failed", getpid());
            return counter;
        }

        if (WIFEXITED(wstatus)) {
            printf("%d: child exited with status %d\n", getpid(), WEXITSTATUS(wstatus));
            break;
        }

        if (WIFSTOPPED(wstatus) && WSTOPSIG(wstatus) == SIGTRAP) {
            // stopped for syscall
#if CONFIG_USE_PTRACE_SYSCALL_INFO
            static struct ptrace_syscall_info syscall_info;

            if (ptrace(PTRACE_GET_SYSCALL_INFO, child, 0, &syscall_info) == -1) {
                error(0, errno, "%d: get syscall info", getpid());
                return counter;
            }

            if (syscall_info.op == PTRACE_SYSCALL_INFO_ENTRY) {
                printf("%d: child enters syscall %lld\n", getpid(), syscall_info.entry.nr);

                switch (syscall_info.entry.nr) {
                case SYS_fork:
                case SYS_vfork:
                case SYS_clone:
                    error(0, 0, "%d: child performed an invalid syscall", getpid());
                    return counter;
                default:
                    ++counter;
                }
            } else if (syscall_info.op == PTRACE_SYSCALL_INFO_EXIT) {
                printf("%d: child leaves syscall with value 0x%llx\n", getpid(),
                        syscall_info.exit.rval);
            }
#else
            static char syscall_enter = 1;
            static struct user_regs_struct regs;

            if (ptrace(PTRACE_GETREGS, child, 0, &regs) == -1) {
                error(0, errno, "%d: cannot read registers", getpid());
            }

            if (syscall_enter) {
                switch (regs.orig_rax) {
                case SYS_fork:
                case SYS_vfork:
                case SYS_clone:
                    error(0, 0, "%d: child performed an invalid syscall", getpid());
                    return counter;
                default:
                    printf("%d: child enters syscall %llu\n", getpid(), regs.orig_rax);
                    ++counter;
                }
            } else {
                printf("%d: child leaves syscall with result 0x%llx\n", getpid(), regs.rax);
            }

            syscall_enter = (syscall_enter + 1) % 2;
#endif
        }

        if (ptrace(PTRACE_SYSCALL, child, 0, NULL) == -1) {
            error(0, 0, "%d: failed to resume child", getpid());
            return counter;
        }
    }

    return counter;
}

size_t run_tracer(pid_t child)
{
    printf("%d: child PID is %d\n", getpid(), child);

    int wstatus;

    // wait for child process to change state
    if (waitpid(child, &wstatus, 0) == -1)
        error(EXIT_FAILURE, errno, "%d: waitpid %d", getpid(), child);

    // did child process stop?
    if (!WIFSTOPPED(wstatus)) {
        error(0, errno, "%d: child did not stop", getpid());
        goto kill_child_and_exit;
    }

    // we can safely set up our ptrace options at this point
    // just for demonstration, kill the child on parent's death

    if (ptrace(PTRACE_SETOPTIONS, child, NULL, (void*) PTRACE_O_EXITKILL) == -1) {
        error(0, errno, "%d: ptrace setup failed", getpid());
        goto kill_child_and_exit;
    }

    // allow child to continue until next syscall
    if (ptrace(PTRACE_SYSCALL, child, 0, NULL) == -1) {
        error(0, errno, "%d: failed to resume child", getpid());
        goto kill_child_and_exit;
    }

    return count_syscalls(child);

kill_child_and_exit:
    kill(child, SIGKILL);
    exit(EXIT_FAILURE);
}

//--  main  -------------------------------------------------------------------

int main(int argc, char *argv[])
{
    if (argc <= 1) {
        usage(argv[0]);
        return EXIT_FAILURE;
    }

    pid_t pid = fork();
    if (pid == -1)
        error(EXIT_FAILURE, errno, "fork");

    if (pid == 0)
        run_tracee(argv[1], argv + 1);

    size_t calls = run_tracer(pid);
    printf("child performed %zu calls before it died\n", calls);
    return EXIT_SUCCESS;
}
