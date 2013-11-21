CC = gcc
AR = ar

CFLAGS = -I ./include -Wall -Werror -Wno-unused -O3 -g

OBJS = $(wildcard src/*.o)
TESTS = $(wildcard tests/*-test)
DEMOS = $(wildcard demos/*-demo)

# For hashmap
OBJS = src/hashmap.o

hashmap-test: $(OBJS)
	$(CC) $(CFLAGS) -o tests/$@  src/hashmap_test.c $(OBJ)

hashmap-demo: $(OBJS)
	$(CC) $(CFLAGS) -o demos/$@  hashmap_demo.c $(OBJ)

 
# Clean
.PHONY: clean 
clean:
	rm -f $(OBJS) $(TESTS) $(DEMOS)
