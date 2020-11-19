SRCS = id.c utils.c
OBJS = $(SRCS:.c=.o)
TARGET = id

override CFLAGS += -std=c99 -pedantic -Wall -Wextra

ifneq ($(strip $(DEBUG)),)
override CFLAGS += -g -ggdb3 -Og
endif

ifneq ($(strip $(OPTIMIZE)),)
override CFLAGS += -O2
endif

all: $(TARGET)

$(TARGET): $(OBJS)

id.o: utils.h
utils.o: utils.h

clean:
	-$(RM) $(OBJS)

distclean: clean
	-$(RM) $(TARGET)

.PHONY: all clean distclean