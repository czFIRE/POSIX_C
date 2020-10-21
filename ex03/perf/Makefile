CFLAGS += -std=c99 -pedantic -Wall

all: fib

fib: fib.o

perf: CFLAGS += -g
perf: clean fib
	perf record -- ./fib 30
	perf report

gprof: CFLAGS += -g -pg
gprof: clean fib
	./fib 30
	gprof ./fib

clean:
	$(RM) *.o gmon.out perf.data*

distclean: clean
	$(RM) fib

.PHONY: all clean distclean perf gprof
