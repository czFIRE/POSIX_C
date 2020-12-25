SRCS = tester_impl.c queue.c tester_thread_safe.c tester_mult_thread.c
OBJS = $(SRCS:.c=.o)
TARGET = tester_impl tester_thread_safe tester_mult_thread

override CFLAGS += -std=c11 -pedantic -Wall -Wextra
override CFLAGS += -pthread
override LDFLAGS += -pthread

ifneq ($(strip $(DEBUG)),)
override CFLAGS += -g -ggdb3 -Og
endif

ifneq ($(strip $(OPTIMIZE)),)
override CFLAGS += -O2
endif

all: $(TARGET)

lib: CFLAGS += -fPIC
lib: LDFLAGS += -Wl,-rpath,. $(origin)
lib: LDLIBS += -ldl
lib: libqueue.so

libqueue.so: queue.o
	$(CC) $(LDFLAGS) -shared -o $@ $^ $(LDLIBS) $(LOADLIBES)

queue.o: queue.h

tester_thread_safe: queue.o
tester_thread_safe.o: queue.h

tester_impl: queue.o
tester_impl.o: queue.h

tester_mult_thread: queue.o
tester_mult_thread.o: queue.h

clean:
	-$(RM) $(OBJS)

distclean: clean
	-$(RM) $(TARGET) libqueue.so

.PHONY: all clean distclean