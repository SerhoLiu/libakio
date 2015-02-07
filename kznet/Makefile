CFLAGS := -std=c99  -pedantic -D NDEBUG -O2 -Wall

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Linux)
	CFLAGS += -D_GNU_SOURCE -D USE_EPOLL -D USE_MONOTONIC
	LIBS := -lrt
endif

OBJS = buffer.o event.o poller.o timer.o socket.o

all: demo

demo: $(OBJS)
	$(CC) $(CFLAGS) -o $@  $(OBJS) demo.c $(LIBS)


.PHONY: clean
clean:
	-rm *.o demo