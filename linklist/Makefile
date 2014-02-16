CC = gcc
AR = ar

CFLAGS = -I ../_test -Wall -Wno-unused -O1 -g


linklist-test: linklist.o
	$(CC) $(CFLAGS) -o $@  linklist_test.c $<

linklist-demo: linklist.o
	$(CC) $(CFLAGS) -o $@  linklist_demo.c $<

# Clean
.PHONY: clean 
clean:
	rm -f linklist-test linklist-demo *.o
