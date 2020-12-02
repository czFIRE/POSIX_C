all: sqrt

debug: override CFLAGS += -g -ggdb3
debug: sqrt

override CFLAGS += -std=c11 -pedantic -Wall -Wextra

sqrt: override CFLAGS += -pthread
sqrt: override LDFLAGS += -pthread
sqrt: sqrt.o

TEST_COUNT ?= 100
TEST_NUMBER ?= 32

clean:
	-$(RM) sqrt.o

distclean: clean
	-$(RM) sqrt

.PHONY: all debug clean distclean
