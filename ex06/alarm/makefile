TARGET = alarm troll
SOURCE = $(TARGET:=.c)
OBJECT = $(TARGET:=.o)
CFLAGS = -std=c99 -pedantic -Wall -Wextra

all: $(TARGET)

alarm: alarm.o
troll: troll.o

.PHONY: all clean

clean:
	$(RM) $(OBJECT)

distclean: clean
	$(RM) $(TARGET)
