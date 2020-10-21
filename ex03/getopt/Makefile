CFLAGS += -std=c99 -pedantic -Wall

all: long short

long: long.o
short: short.o

clean:
	$(RM) *.o

distclean: clean
	$(RM) long short

.PHONY: all clean distclean
