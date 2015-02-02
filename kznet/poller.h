#ifndef _KZNET_POLLER_H_
#define _KZNET_POLLER_H_

#include "event.h"


struct ev_event {
    int  event;
    
    void *data;

    event_callback *read;  
    event_callback *write; 
};

struct ev_active {
    int fd;
    int event;
};


int poller_create(event_loop_t *loop);

void poller_delete(event_loop_t *loop);

int poller_addevent(event_loop_t *loop, int fd, int event);

int poller_delevent(event_loop_t *loop, int fd, int event);

int poller_poll(event_loop_t *loop, int timeout);

char *poller_name();

#endif
