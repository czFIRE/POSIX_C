SOURCE = test.c stopwatch.c
OBJECT = $(SOURCE:.c=.o)
TARGET = test

CFLAGS = -std=c99 -pedantic -Wall -Wextra
CFLAGS += -pthread
LDFLAGS += -pthread

all: $(TARGET)

$(TARGET): $(OBJECT)

test.o: stopwatch.h
stopwatch.o: stopwatch.h

clean:
	$(RM) $(OBJECT)

distclean: clean
	$(RM) $(TARGET)

.PHONY: all clean distclean detach