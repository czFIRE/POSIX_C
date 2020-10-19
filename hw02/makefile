CFLAGS = -std=c99 -Wall -Wextra -g

all: lib bin

##########################

libbinary.o liblinear.o liboracle.o: strategy.h

##########################

lib: CFLAGS += -fPIC
lib: LDFLAGS += -Wl,-rpath,.
lib: libbinary.so liblinear.so liboracle.so

%.so: %.o
	$(CC) $(LDFLAGS) -shared -o $@ $^ $(LDLIBS) $(LOADLIBES)

##########################

bin: LDFLAGS =-Wl,-rpath,.
bin: LDLIBS = -ldl
bin: guess

guess.o utils.o: utils.h

guess: utils.o

##########################

clean: 
	$(RM) *.o *.a

distclean:
	$(RM) guess *.o *.a *.so

.PHONY: all clean lib bin