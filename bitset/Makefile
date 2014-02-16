CC = gcc
AR = ar

CFLAGS = -I ../_test -Wall -Wno-unused -O1 -g


bitset-test: bitset.o
	$(CC) $(CFLAGS) -o $@  bitset_test.c $<

bitset-demo: bitset.o
	$(CC) $(CFLAGS) -o $@  bitset_demo.c $<

# Clean
.PHONY: clean 
clean:
	rm -f bitset-test bitset-demo *.o
