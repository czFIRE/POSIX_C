BINS = await suspend syncsig
OBJS = $(BINS:=.o)

all: $(BINS)

await: 		await.o
suspend: 	suspend.o
syncsig:	syncsig.o

.PHONY: all clean

clean:
	$(RM) $(BINS) *.o
