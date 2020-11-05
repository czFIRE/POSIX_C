TARGET = spooky
SOURCE = spooky.c
OBJECT = spooky.o
CFLAGS = -std=c99 -pedantic -Wall -Wextra

all: $(TARGET)

$(TARGET): $(OBJECT)
$(OBJECT): $(SOURCE)

.PHONY: all clean

clean:
	$(RM) $(TARGET) $(OBJECT)
