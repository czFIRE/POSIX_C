SRCS = crc.c crc32check.c crc32sum.c get_crc.c
OBJS = $(SRCS:.c=.o)
TARGET = crc32sum crc32check

#CFLAGS = -std=c99 -pedantic -Wall -Wextra

ifneq ($(strip $(DEBUG)),)
	CFLAGS += -g -ggdb3 -Og
endif

ifneq ($(strip $(OPTIMIZE)),)
	CFLAGS += -O2
endif

all: $(TARGET)

$(TARGET): crc.o get_crc.o

crc.o: crc.h
crc32check.o: crc.h get_crc.h
crc32sum.o: crc.h get_crc.h
get_crc.o: constants.h get_crc.h crc.h

clean:
	-$(RM) $(OBJS)

distclean:
	-$(RM) $(OBJS) $(TARGET)

.PHONY: all clean distclean
