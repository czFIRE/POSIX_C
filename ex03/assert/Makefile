CFLAGS += -std=c99 -pedantic -Wall

all: assert-d assert-nd

assert-d.o: assert.c
	$(CC) $(CFLAGS) -c $(CPPFLAGS) -o $@ $<

assert-d: assert-d.o

assert-nd.o: CPPFLAGS += -DNDEBUG
assert-nd.o: assert.c
	$(CC) $(CFLAGS) -c $(CPPFLAGS) -o $@ $<

assert-nd: assert-nd.o

clean:
	$(RM) *.o

distclean: clean
	$(RM) assert-{,n}d{,.o}

.PHONY: all clean distclean
