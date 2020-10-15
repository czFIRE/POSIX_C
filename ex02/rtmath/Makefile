TARGETS = rtmath32 rtmath64
CFLAGS  = -std=c99
LDLIBS  = -ldl

all: $(TARGETS)

# create both 32bit and 64bit versions
rtmath32: LDFLAGS := -m32
rtmath64: LDFLAGS := -m64
rtmath%: rtmath%.o

rtmath%.o: rtmath.c
	$(CC) $(CFLAGS) -m$* -c -o $@ $<

.PHONY: all clean
clean:
	$(RM) $(TARGETS) *.o
