all: primes

debug: override CFLAGS += -g -ggdb3
debug: primes

override CFLAGS += -std=c11 -pedantic -Wall -Wextra

primes: override CFLAGS += -pthread
primes: override LDFLAGS += -pthread
primes: primes.o list.o

primes.o: list.h
list.o: list.h

clean:
	-$(RM) primes.o list.o

distclean: clean
	-$(RM) primes

.PHONY: all debug clean distclean
