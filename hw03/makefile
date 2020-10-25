CFLAGS = -std=c99 -Wall -Wextra -g

all: lib bin

##########################

libmemgrind.o: libmemgrind.h #utils.h
#utils.o: utils.h

##########################

lib: CFLAGS += -fPIC
lib: LDFLAGS += -Wl,-rpath,.
lib: libmemgrind.so

libmemgrind.so: libmemgrind.o #utils.o
	$(CC) $(LDFLAGS) -shared -o $@ $^ $(LDLIBS) $(LOADLIBES)

##########################

bin: test_easy

##########################

clean: 
	$(RM) *.o *.a

distclean: clean
	$(RM) *.so test_easy

.PHONY: all clean lib bin