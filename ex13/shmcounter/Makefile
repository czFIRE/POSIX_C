TARGET = shmcounter
SOURCE = $(TARGET).c
OBJECT = $(SOURCE:.c=.o)

CFLAGS ?= -std=gnu99 -Wall -Wextra

all: $(TARGET)
$(TARGET): $(OBJECT)

clean:
	$(RM) $(OBJECT)

distclean: clean
	$(RM) $(TARGET)

.PHONY: all clean distclean
