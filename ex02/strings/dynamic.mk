#pozor, z nazvu knihoven se odebira lib
CFLAGS = -std=c99

all: lib bin

##########################

# could be replaced with %.o: libfilter.h
liblc.o: libfilter.h
libuc.o: libfilter.h
libinfo.o: libfilter.h

##########################

lib: CFLAGS += -fPIC
lib: LDFLAGS += -Wl,-rpath,.
lib: libuc.so liblc.so libinfo.so

libuc.so: libuc.o
	$(CC) $(LDFLAGS) -shared -o $@ $^ $(LDLIBS) $(LOADLIBES)

liblc.so: liblc.o
	$(CC) $(LDFLAGS) -shared -o $@ $^ $(LDLIBS) $(LOADLIBES)

libinfo.so: libinfo.o
	$(CC) $(LDFLAGS) -shared -o $@ $^ $(LDLIBS) $(LOADLIBES)

##########################

bin: LDFLAGS =-Wl,-rpath,.
bin: LDLIBS = -ldl
bin: strfilter

strfilter.o: libfilter.h

##########################

clean: 
	$(RM) strfilter *.o *.a *.so *.0 *.1

.PHONY: all clean lib bin