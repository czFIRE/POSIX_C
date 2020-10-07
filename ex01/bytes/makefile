CFLAGS = -std=c99 -Wall -Wextra -pedantic

all: bytes

debug: CFLAGS += -g
debug: all

bytes: bytes.o
%.o: 

clean: 
	rm -f bytes *.o

.PHONY: all clean