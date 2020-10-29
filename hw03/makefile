CFLAGS = -std=c99 -Wall -Wextra -g

all: lib bin

##########################

libmemgrind.o: libmemgrind.h utils.h syslog_parser.h
utils.o: utils.h syslog_parser.h
syslog_parser.o: syslog_parser.h

##########################

lib: CFLAGS += -fPIC
lib: LDFLAGS += -Wl,-rpath,. $(origin)
lib: LDLIBS += -ldl
lib: libmemgrind.so

libmemgrind.so: libmemgrind.o utils.o syslog_parser.o
	$(CC) $(LDFLAGS) -shared -o $@ $^ $(LDLIBS) $(LOADLIBES)

##########################

bin: test_easy
bin: test_realloc

##########################

clean: 
	$(RM) *.o *.a

distclean: clean
	$(RM) *.so test_easy test_realloc

.PHONY: all clean lib bin