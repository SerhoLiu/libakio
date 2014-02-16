CC = gcc
AR = ar

CFLAGS = -I ../_test -Wall -Wno-unused -O1 -g


skiplist-test: skiplist.o
	$(CC) $(CFLAGS) -o $@  skiplist_test.c $<

skiplist-demo: skiplist.o
	$(CC) $(CFLAGS) -o $@  skiplist_demo.c $<

# Clean
.PHONY: clean 
clean:
	rm -f skiplist-test skiplist-demo *.o
