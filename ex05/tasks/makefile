SRCS = tasks.c dynarray.c
OBJS = $(SRCS:.c=.o)
TARGET = tasks

override CFLAGS += -std=c99 -Wall -Wextra -g

all: $(TARGET)

$(TARGET): dynarray.o

$(SRCS): dynarray.h

clean:
	-$(RM) $(OBJS)

distclean:
	-$(RM) $(OBJS) $(TARGET)

.PHONY: all clean distclean test
