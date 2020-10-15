#pozor, z nazvu knihoven se odebira lib
LDLIBS = -lnum
LDFLAGS = -L.

all: lib bin

bin: fact comb
#fact: fact.o #| libfact.a

fact.o: num.h
libfact.o: num.h

##########################

comb.o: num.h
libcomb.o: num.h

##########################

lib: libnum.a

libnum.a: libfact.o libcomb.o
	$(AR) rsv $@ $^

##########################

clean: 
	$(RM) comb fact *.o *.a

.PHONY: all clean lib bin