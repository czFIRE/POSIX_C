TARGET = abort
SOURCE = $(TARGET:=.c)
OBJECT = $(TARGET:=.o)
CFLAGS = -std=c99 -pedantic -Wall -Wextra

all: $(TARGET)

$(TARGET): $(OBJECT)
$(OBJECT): $(SOURCE)

.PHONY: all clean

clean:
	$(RM) $(OBJECT)

distclean: clean
	$(RM) $(TARGET)
