TARGET = syscount
SOURCE = $(TARGET:=.c)
OBJECT = $(TARGET:=.o)
CFLAGS = -std=c99 -pedantic -Wall -Wextra

all: $(TARGET)

$(TARGET): $(OBJECT)
$(OBJECT): $(SOURCE)

no_syscall_info: CPPFLAGS += -DCONFIG_USE_PTRACE_SYSCALL_INFO=0
no_syscall_info: $(TARGET)

.PHONY: all clean

clean:
	$(RM) $(OBJECT)

distclean: clean
	$(RM) $(TARGET)
