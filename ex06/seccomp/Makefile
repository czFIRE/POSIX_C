CFLAGS = -std=gnu99 -Wall -Wextra

all: seccomp
run: seccomp
	./seccomp

seccomp: LDLIBS += -lseccomp
seccomp: seccomp.o

clean:
	$(RM) seccomp.o .tmp.*

distclean: clean
	$(RM) seccomp

.PHONY: all clean distclean
