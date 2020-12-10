TARGETS = slow fast

all: $(TARGETS)

slow: slow.o
fast: fast.o

clean:
	$(RM) *.o

distclean: clean
	$(RM) $(TARGETS)

.PHONY: all clean distclean
