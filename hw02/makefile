CFLAGS = -std=c99 -Wall -Wextra -g

all: lib bin

##########################

libbinary.o: strategy.h
liblinear.o: strategy.h
liboracle.o: strategy.h

##########################

lib: CFLAGS += -fPIC
lib: LDFLAGS += -Wl,-rpath,.
lib: libbinary.so liblinear.so liboracle.so

# maybe replace with only 1 line
libbinary.so: libbinary.o
	$(CC) $(LDFLAGS) -shared -o $@ $^ $(LDLIBS) $(LOADLIBES)

liblinear.so: liblinear.o
	$(CC) $(LDFLAGS) -shared -o $@ $^ $(LDLIBS) $(LOADLIBES)

liboracle.so: liboracle.o
	$(CC) $(LDFLAGS) -shared -o $@ $^ $(LDLIBS) $(LOADLIBES)

##########################

bin: LDFLAGS =-Wl,-rpath,.
bin: LDLIBS = -ldl
bin: guess

guess.o: strategy.h

##########################

clean: 
	$(RM) guess *.o *.a *.so

.PHONY: all clean lib bin