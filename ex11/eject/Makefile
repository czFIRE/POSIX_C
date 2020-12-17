TARGET = eject
SRC = $(TARGET).c
OBJ = $(SRC:.c=.o)

CFLAGS ?= -std=c99 -pedantic -Wall -Wextra

all: $(TARGET)

$(TARGET): $(OBJ)

clean:
	$(RM) $(OBJ)

distclean: clean
	$(RM) $(TARGET)

.PHONY: all clean distclean
