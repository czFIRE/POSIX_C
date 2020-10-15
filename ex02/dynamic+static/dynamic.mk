#pozor, z nazvu knihoven se odebira lib
#LDLIBS = -lnum
#LDFLAGS = -L.
CFLAGS = -std=c99

MAJOR = 1
MINOR = 0

LDNAME = libnum.so
SONAME = $(LDNAME).$(MAJOR)
REALNAME = $(LDNAME).$(MAJOR).$(MINOR)

all: lib symlinks bin
#fact: fact.o #| libfact.a

##########################

fact.o: num.h
libfact.o: num.h

comb.o: num.h
libcomb.o: num.h

##########################

lib: CFLAGS += -fPIC
lib: $(REALNAME)

$(REALNAME): LDFLAGS += -Wl,-soname,$(SONAME)
$(REALNAME): libfact.o libcomb.o
	$(CC) $(LDFLAGS) -shared -o $@ $^ $(LDLIBS) $(LOADLIBES)

##########################


bin: LDFLAGS =-L . -Wl,-rpath,.
bin: LDLIBS = -lnum
bin: fact comb

##########################

symlinks: $(REALNAME)
# ldconfig does not set up LDNAME
	ln -sf $< $(LDNAME)
# but it creates SONAME -> REALNAME
	ldconfig -n .

##########################

clean: 
	$(RM) comb fact *.o *.a *.so *.0 *.1

.PHONY: all clean lib bin