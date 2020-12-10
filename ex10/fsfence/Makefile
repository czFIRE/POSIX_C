CFLAGS = -std=gnu99 -Wall -Wextra

all: fsfence

fsfence: LDLIBS += -lseccomp
fsfence: fsfence.o

clean:
	$(RM) fsfence.o .tmp.*

distclean: clean
	$(RM) fsfence

.PHONY: all clean distclean
