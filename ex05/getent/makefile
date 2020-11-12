SRCS = getent.c
OBJS = $(SRCS:.c=.o)
TARGET = getent

override CFLAGS += -std=c99 -Wall -Wextra -g

all: $(TARGET)

$(TARGET): $(OBJS)

clean:
	-$(RM) $(OBJS)

distclean:
	-$(RM) $(OBJS) $(TARGET)

.PHONY: all clean distclean test
