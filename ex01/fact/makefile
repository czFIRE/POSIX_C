# SOURCES = fact.c lib.c
# OBJECTS = $(SOURCES:.c=.o)

# promenne se expanduji az tehdy, co se pouziji
all: fact

#fact: fact.o lib.o
fact: lib.o

# c soubory si domysli
%.o: %.h

#fact.o: fact.c lib.h

clean: 
	$(RM) fact *.o

.PHONY: all clean

# %: %.o
# $(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS) $(LOADLIBES)