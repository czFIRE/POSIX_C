SRCS = easy_tester.c queue.c
OBJS = $(SRCS:.c=.o)
TARGET = easy_tester

override CFLAGS += -std=c11 -pedantic -Wall -Wextra

ifneq ($(strip $(DEBUG)),)
override CFLAGS += -g -ggdb3 -Og
endif

ifneq ($(strip $(OPTIMIZE)),)
override CFLAGS += -O2
endif

all: $(TARGET)

$(TARGET): override CFLAGS += -pthread
$(TARGET): override LDFLAGS += -pthread
$(TARGET): $(OBJS)

#tester.o: queue.h
easy_tester.o: queue.h
queue.o: queue.h

clean:
	-$(RM) $(OBJS)

distclean: clean
	-$(RM) $(TARGET)

.PHONY: all clean distclean