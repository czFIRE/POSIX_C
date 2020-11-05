CFLAGS += -std=c99 -Wall -Wextra
SUDO   ?= sudo

all: drop setuid

drop: drop.o

warn:
	@echo -e "\e[91mWarning:\e[0m If you don't know what is going on, press \e[1mCtrl-C\e[0m"
	@echo -e "and inspect the '$@' target in 'Makefile'!"

setuid: drop
	$(SUDO) chown 0 $<
	$(SUDO) chmod ug+s $<

clean:
	$(RM) *.o

distclean: clean
	$(RM) drop

.PHONY: all setuid clean distclean
