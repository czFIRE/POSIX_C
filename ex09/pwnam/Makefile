TARGET  = pwnam
OBJ     = $(TARGET).o
SRC     = $(TARGET).c
LDFLAGS = -pthread

all: $(TARGET)

run: $(TARGET)
	@for I in {1..1000}; \
		do ./$(TARGET) | grep -E '^0:' | grep -v 'root'; \
	done

$(TARGET): $(OBJ)
$(OBJ): $(SRC)

.PHONY: all clean run

clean:
	$(RM) $(TARGET) $(OBJ)
