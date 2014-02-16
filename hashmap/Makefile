CC = gcc
AR = ar

CFLAGS = -I ../_test -Wall -Wno-unused -O1 -g


hashmap-test: hashmap.o
	$(CC) $(CFLAGS) -o $@  hashmap_test.c $<

hashmap-demo: hashmap.o
	$(CC) $(CFLAGS) -o $@  hashmap_demo.c $<

# Clean
.PHONY: clean 
clean:
	rm -f hashmap-test hashmap-demo *.o
