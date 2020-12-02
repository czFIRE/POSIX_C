all: condvar

debug: override CFLAGS += -g -ggdb3
debug: condvar

override CFLAGS += -std=c11 -pedantic -Wall -Wextra

condvar: override CFLAGS += -pthread
condvar: override LDFLAGS += -pthread
condvar: condvar.o

clean:
	-$(RM) condvar.o

distclean: clean
	-$(RM) condvar

.PHONY: all debug clean distclean
