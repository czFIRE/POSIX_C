TARGET = ptgrep
SOURCE = $(TARGET:=.c)
OBJECT = $(TARGET:=.o)
CFLAGS = -std=c99 -pedantic -Wall -Wextra
CFLAGS += -pthread
LDFLAGS += -pthread

all: $(TARGET)

$(TARGET): $(OBJECT)
$(OBJECT): $(SOURCE)

clean:
	$(RM) $(OBJECT)

distclean: clean
	$(RM) $(TARGET)

.PHONY: all clean distclean detach