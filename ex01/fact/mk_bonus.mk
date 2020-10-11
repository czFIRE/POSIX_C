SRCS = fact.c lib.c
DEPS = $(SRCS:.c=.d)

all: fact

fact: fact.o lib.o $(DEPS)

%.d: %.c
	$(CC) -MM -MF $@ $<

include $(DEPS)