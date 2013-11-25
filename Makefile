CC = gcc
AR = ar

CFLAGS = -I ./include -Wall -Wno-unused -O3 -g

ALLOBJS = $(wildcard src/*.o)
TESTS = $(wildcard tests/*-test)
DEMOS = $(wildcard demos/*-demo)

# For hashmap
src/hashmap.o: src/hashmap.c
	$(CC) $< -c $(CFLAGS) -o $@ 

hashmap-test: src/hashmap.o
	$(CC) $(CFLAGS) -o tests/$@  tests/hashmap_test.c src/hashmap.o

hashmap-demo: src/hashmap.o
	$(CC) $(CFLAGS) -o demos/$@  demos/hashmap_demo.c src/hashmap.o

# For bitset
src/bitset.o: src/bitset.c
	$(CC) $< -c $(CFLAGS) -o $@

bitset-test: $(OBJS)
	$(CC) $(CFLAGS) -o tests/$@  tests/bitset_test.c src/bitset.o

bitset-demo: $(OBJS)
	$(CC) $(CFLAGS) -o demos/$@  demos/bitset_demo.c src/bitset.o

# For BloomFilter
src/bloomfilter.o: src/bloomfilter.c
	$(CC) $< -c $(CFLAGS) -o $@

bloomfilter-test: src/bloomfilter.o src/bitset.o
	$(CC) $(CFLAGS) -o tests/$@  tests/bloomfilter_test.c src/bloomfilter.o src/bitset.o -lm

bloomfilter-demo: src/bloomfilter.o src/bitset.o
	$(CC) $(CFLAGS) -o demos/$@  demos/bloomfilter_demo.c src/bloomfilter.o src/bitset.o -lm
 
# Clean
.PHONY: clean 
clean:
	rm -f $(ALLOBJS) $(TESTS) $(DEMOS)
