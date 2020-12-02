all: power

debug: override CFLAGS += -g -ggdb3
debug: power

override CFLAGS += -std=c11 -pedantic -Wall -Wextra

power: override CFLAGS += -pthread
power: override LDFLAGS += -pthread
power: power.o

TEST_COUNT ?= 100
TEST_NUMBER ?= 32

test: power
	@FAILED=$$(for I in {1..$(TEST_COUNT)}; do				\
	    if ! ./power $(TEST_NUMBER) >/dev/null; then		\
		echo "iteration $$I: ./test $(TEST_NUMBER) failed";	\
	    fi								\
	done | wc -l);							\
	echo "$$FAILED/$(TEST_COUNT) iterations failed";		\
	test "$$FAILED" -eq 0

clean:
	-$(RM) power.o

distclean: clean
	-$(RM) power

.PHONY: all debug test clean distclean
