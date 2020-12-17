# Set up target
TARGET = sha256attr

# General compiler flags
CFLAGS  += -std=gnu99 -Wall -Wextra

# Sources and objects
SOURCES = $(shell find $(CURDIR) -maxdepth 1 -type f -name '*.c' -printf '%P\n')
OBJECTS = $(SOURCES:%.c=%.o)

# Compiler and linker flags for OpenSSL
CFLAGS  += $(shell pkg-config --cflags openssl)
LDFLAGS += $(shell pkg-config --libs-only-L openssl)
LDLIBS  += $(shell pkg-config --libs-only-l openssl)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $^ $(LDLIBS)

# Header dependencies
sha_utils.o: sha_utils.h

clean:
	$(RM) -r $(OBJECTS) $(DEPDIR)

distclean: clean
	$(RM) $(TARGET)

.PHONY: all clean distclean
