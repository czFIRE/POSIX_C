BINS = child dup
OBJS = $(BINS:=.o)

all: $(BINS)

child:	child.o
dup:	dup.o

clean:
	$(RM) *.o

distclean: clean
	$(RM) $(BINS)

.PHONY: all clean distclean
