#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include "poller.h"


#ifdef USE_EPOLL

#include <sys/epoll.h>

struct poller {
    int epfd;

    struct epoll_event *events;
};


int poller_create(event_loop_t *loop)
{
    struct poller *poller;

    poller = malloc(sizeof(*poller));
    if (poller == NULL) return -1;
    
    poller->events = malloc(sizeof(struct epoll_event) * loop->setsize);
    if (poller->events == NULL) {
        free(poller);
        return -1;
    }

     /* 1024 is just an hint for the kernel */
    poller->epfd = epoll_create(1024);
    if (poller->epfd == -1) {
        free(poller->events);
        free(poller);
        return -1;
    }

    loop->poller = poller;
    
    return 0;
}

void poller_delete(event_loop_t *loop)
{
    struct poller *poller;

    poller = loop->poller;
    assert(poller != NULL);

    close(poller->epfd);
    free(poller->events);
    free(poller);
}

int poller_addevent(event_loop_t *loop, int fd, int event)
{
    int op;

    struct poller     *poller;
    struct epoll_event ee;
    
    poller = loop->poller;
    assert(poller != NULL);
    
    op = loop->events[fd].event == EV_NONE ? EPOLL_CTL_ADD : EPOLL_CTL_MOD;
    
    ee.events = 0;
    event |= loop->events[fd].event;
    if (event & EV_RDABLE) ee.events |= EPOLLIN;
    if (event & EV_WRABLE) ee.events |= EPOLLOUT;
    ee.data.u64 = 0;
    ee.data.fd = fd;
    
    return epoll_ctl(poller->epfd, op, fd, &ee);
}

int poller_delevent(event_loop_t *loop, int fd, int event)
{
    int op;
    int nevent;

    struct poller     *poller;
    struct epoll_event ee;
    
    poller = loop->poller;
    assert(poller != NULL);
    
    nevent = loop->events[fd].event & (~event);
    op = nevent == EV_NONE ? EPOLL_CTL_DEL : EPOLL_CTL_MOD;

    ee.events = 0;
    if (nevent & EV_RDABLE) ee.events |= EPOLLIN;
    if (nevent & EV_WRABLE) ee.events |= EPOLLOUT;
    ee.data.u64 = 0;
    ee.data.fd = fd;

    return epoll_ctl(poller->epfd, op, fd, &ee);
}

int poller_poll(event_loop_t *loop, int timeout)
{
    int j;
    int event;
    int retval, numevents;

    struct poller      *poller;
    struct epoll_event *e;
    
    poller = loop->poller;
    assert(poller != NULL);
    
    numevents = 0;
    retval = epoll_wait(poller->epfd, poller->events, loop->setsize, timeout);
    if (retval > 0) {

        numevents = retval;

        for (j = 0; j < numevents; j++) {
            event = EV_NONE;
            e = poller->events+j;

            if (e->events & EPOLLIN)  event |= EV_RDABLE;
            if (e->events & EPOLLOUT) event |= EV_WRABLE;
            if (e->events & EPOLLERR) event |= EV_WRABLE;
            if (e->events & EPOLLHUP) event |= EV_WRABLE;
            
            loop->actives[j].fd = e->data.fd;
            loop->actives[j].event = event;
        }
    }
    
    return numevents;
}

static char *ev_api_name(void)
{
    return "epoll";
}

#else


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
    int j;
    int event;
    int retval, numevents;

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

#endif
