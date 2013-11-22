CC = gcc
AR = ar

CFLAGS = -I ./include -Wall -Wno-unused -O3 -g

OBJS = $(wildcard src/*.o)
TESTS = $(wildcard tests/*-test)
DEMOS = $(wildcard demos/*-demo)

# For hashmap
#OBJS = src/hashmap.o

hashmap-test: $(OBJS)
	$(CC) $(CFLAGS) -o tests/$@  tests/hashmap_test.c $(OBJS)

hashmap-demo: $(OBJS)
	$(CC) $(CFLAGS) -o demos/$@  demos/hashmap_demo.c $(OBJS)

 
# Clean
.PHONY: clean 
clean:
	rm -f $(OBJS) $(TESTS) $(DEMOS)
