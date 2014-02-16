CC = gcc
AR = ar

CFLAGS = -I ../_test -I ../bitset -Wall -Wno-unused -O1 -g

bitset.o: ../bitset/bitset.c
	$(CC) $< -c $(CFLAGS) -o $@

bloomfilter-test: bloomfilter.o bitset.o
	$(CC) $(CFLAGS) -o $@  bloomfilter_test.c bloomfilter.o bitset.o -lm

bloomfilter-demo: bloomfilter.o bitset.o
	$(CC) $(CFLAGS) -o $@  bloomfilter_demo.c bloomfilter.o bitset.o -lm

# Clean
.PHONY: clean 
clean:
	rm -f bloomfilter-test bloomfilter-demo *.o
