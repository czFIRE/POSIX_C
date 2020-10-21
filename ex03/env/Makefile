CPPFLAGS += -D_DEFAULT_SOURCE
CFLAGS += -std=c99 -pedantic -Wall

TARGETS = setenv putenv
all: $(TARGETS)

setenv: setenv.o
putenv: putenv.o

clean:
	$(RM) *.o

distclean: clean
	$(RM) $(TARGETS)

.PHONY: all clean distclean
