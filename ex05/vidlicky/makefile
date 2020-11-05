SRCS = vidlicky.c
OBJS = $(SRCS:.c=.o)
TARGET = vidlicky

override CFLAGS += -std=c99 -Wall -Wextra -g

all: $(TARGET)

#$(TARGET): 

test:
	for I in `seq 20`; do \
		echo "-------------------------------" ; \
		./vidlicky ; \
	done

wait: CFLAGS += -D ENABLE_WAIT
wait: all

clean:
	-$(RM) $(OBJS)

distclean:
	-$(RM) $(OBJS) $(TARGET)

.PHONY: all clean distclean test
