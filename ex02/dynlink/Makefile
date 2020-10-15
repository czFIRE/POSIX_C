CFLAGS = -std=c99 -pedantic -Wall -Wextra
TARGET = factorial

# Library version
MAJOR = 2
MINOR = 0

# These variables are not usually used in Makefile, they are here only for
# the sake of the example
LDNAME = libfact.so
SONAME = $(LDNAME).$(MAJOR)
REALNAME = $(LDNAME).$(MAJOR).$(MINOR)

all: $(REALNAME) symlinks $(TARGET)

# libfact.o should be a position independent code
fact.o: CFLAGS += -fPIC
fact.o: fact.h

# When linking the library, it will be saved in its RealName and its -soname
# will be set to SOName
$(REALNAME): LDFLAGS += -Wl,-soname,$(SONAME)

# Object file does not match the target, so we will tell Make how to make
# it explicitly
$(REALNAME): fact.o
	$(CC) $(LDFLAGS) -shared -o $@ $^ $(LDLIBS) $(LOADLIBES)

# We will now create symbolic links for LinkerName and SOName
#
# -----------------------------------------------------------
# THIS IS NOT USUALLY DONE BY MAKEFILE
# There is tool, ldconfig, which manages symlinks for libraries
# -----------------------------------------------------------
symlinks: $(REALNAME)
# ldconfig does not set up LDNAME
	ln -sf $< $(LDNAME)
# but it creates SONAME -> REALNAME
	ldconfig -n .
# which is essentially same as
# 	ln -sf $< $(SONAME)

# This will look for library called libfact.so in the current directory
$(TARGET): LDFLAGS = -L . -Wl,-rpath,.
$(TARGET): LDLIBS = -lfact
$(TARGET): factorial.o

# factorial.o will be created using an implicit rule
factorial.o: fact.h

# If target X is a dependency of a special target .PHONY, then X will ALWAYS
# be recreated even if file called X exists and is newer than its dependencies
.PHONY: all clean distclean symlinks

clean:
	$(RM) *.o factorial

distclean: clean
	$(RM) $(shell find . -type l -o \( -type f -a -executable \) )
