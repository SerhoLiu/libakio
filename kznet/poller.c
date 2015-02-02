#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include "poller.h"

#include <sys/select.h>


struct poller {
    fd_set rfds, wfds;
    fd_set crfds, cwfds;
};


int poller_create(event_loop_t *loop)
{
    struct poller *poller;

    poller = malloc(sizeof(*poller));
    if (poller == NULL) return -1;
    
    FD_ZERO(&poller->rfds);
    FD_ZERO(&poller->wfds);
    loop->poller = poller;

    return 0;
}

void poller_delete(event_loop_t *loop)
{
    free(loop->poller);
}

int poller_addevent(event_loop_t *loop, int fd, int event)
{
    struct poller *poller;

    poller = loop->poller;
    assert(poller != NULL);
    
    if (event & EV_RDABLE) FD_SET(fd, &poller->rfds);
    if (event & EV_WRABLE) FD_SET(fd, &poller->wfds);

    return 0;
}

int poller_delevent(event_loop_t *loop, int fd, int event)
{
    struct poller *poller;

    poller = loop->poller;
    assert(poller != NULL);

    if (event & EV_RDABLE) FD_CLR(fd, &poller->rfds);
    if (event & EV_WRABLE) FD_CLR(fd, &poller->wfds);

    return 0;
}

int poller_poll(event_loop_t *loop, int timeout)
{
    
    int event;
    int retval, j, numevents;

    struct poller   *poller;
    struct timeval  tv;
    struct ev_event *ev;

    numevents = 0;
    poller = loop->poller;
    assert(poller != NULL);

    memcpy(&poller->crfds, &poller->rfds, sizeof(fd_set));
    memcpy(&poller->cwfds, &poller->wfds, sizeof(fd_set));

    tv.tv_sec = timeout / 1000;
    tv.tv_usec = (timeout % 1000) * 1000;

    retval = select(loop->maxfd + 1, &poller->crfds, &poller->cwfds, NULL, &tv);
    if (retval > 0) {

        for (j = 0; j <= loop->maxfd; j++) {
            
            event = EV_NONE;
            ev = &loop->events[j];

            if (ev->event == EV_NONE) continue;
            if (ev->event & EV_RDABLE && FD_ISSET(j, &poller->crfds)) {
                event |= EV_RDABLE;
            }
            if (ev->event & EV_WRABLE && FD_ISSET(j, &poller->cwfds)) {
                event |= EV_WRABLE;
            }
            
            loop->actives[numevents].fd = j;
            loop->actives[numevents].event = event;
            
            numevents++;
        }
    }

    return numevents;
}

char *poller_name()
{
    return "select";
}
