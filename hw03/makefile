CFLAGS = -std=c99 -Wall -Wextra -g

all: lib bin

##########################

libmemgrind.o: libmemgrind.h utils.h
utils.o: utils.h

##########################

lib: CFLAGS += -fPIC
lib: LDFLAGS += -Wl,-rpath,.
lib: libmemgrind.so

libmemgrind.so: libmemgrind.o utils.o
	$(CC) $(LDFLAGS) -shared -o $@ $^ $(LDLIBS) $(LOADLIBES)

##########################

clean: 
	$(RM) *.o *.a

distclean:
	$(RM) *.o *.a *.so

.PHONY: all clean lib bin