all: 
	@echo $(.ALLTARGETS)

cpp: lib.c
	$(CC) -std=c99 -E $< -o lib.i

ASMFLAGS = -std=c99 -S -fverbose-asm

cc: cpp O0 O1 O2 O3
	$(CC) $(ASMFLAGS) lib.i -o lib.s

O0: lib.i
	$(CC) $(ASMFLAGS) -O0 $< -o lib-0.s

O1: lib.i
	$(CC) $(ASMFLAGS) -O1 $< -o lib-1.s

O2: lib.i
	$(CC) $(ASMFLAGS) -O2 $< -o lib-2.s

O3: lib.i
	$(CC) $(ASMFLAGS) -O3 $< -o lib-3.s 

clean:
	rm -f lib.i lib*.s