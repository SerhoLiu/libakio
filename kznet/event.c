#include <stdlib.h>
#include <assert.h>

#include "poller.h"
#include "timer.h"
#include "event.h"


event_loop_t *event_loop_create(int maxevents, int maxtimers)
{
    int i;
    event_loop_t *loop;
    
    loop = malloc(sizeof(*loop));
    if (loop == NULL) return NULL;

    loop->timers = timer_heap_create(maxtimers);
    if (loop->timers == NULL) {
        free(loop);
        return NULL;
    }

    loop->setsize = maxevents;
    loop->stop = 0;
    loop->maxfd = -1;

    loop->events = malloc(sizeof(struct ev_event) * maxevents);
    loop->actives =  malloc(sizeof(struct ev_active) * maxevents);
    if (loop->events == NULL || loop->actives == NULL) {
        goto error;
    }

    if (poller_create(loop) == -1) {
        goto error;
    }

    for (i = 0; i < maxevents; i++) {
        loop->events[i].event = EV_NONE;
    }

    return loop;

error:
    timer_heap_delete(loop->timers);
    free(loop->events);
    free(loop->actives);
    free(loop);
    return NULL;
}

void event_loop_delete(event_loop_t *loop)
{
    poller_delete(loop);
    timer_heap_delete(loop->timers);
    free(loop->events);
    free(loop->actives);
    free(loop);
}

int create_event(event_loop_t *loop,
                 int fd,
                 int event, 
                 event_callback *cb,
                 void *data)
{
    struct ev_event *ev;

    assert(loop != NULL);
    assert(cb != NULL);

    if (fd < 1 || fd >= loop->setsize) return -1;
    
    ev = &loop->events[fd];

    if (poller_addevent(loop, fd, event) != 0) return -1;

    ev->event |= event;

    if (event & EV_RDABLE) ev->read = cb;
    if (event & EV_WRABLE) ev->write = cb;
    ev->data = data;

    if (fd > loop->maxfd) loop->maxfd = fd;

    return 0;
}

int delete_event(event_loop_t *loop, int fd, int event)
{
    int j;
    struct ev_event *ev;

    assert(loop != NULL);

    if (fd >= loop->setsize) return -1;
    
    ev = &loop->events[fd];
    if (ev->event == EV_NONE) return 0;

    if (poller_delevent(loop, fd, event) != 0) return -1;

    ev->event = ev->event & (~event);

    /* Update the max fd */
    if (fd == loop->maxfd && ev->event == EV_NONE) {
        for (j = loop->maxfd-1; j >= 0; j--) {
            if (loop->events[j].event != EV_NONE) break;
            loop->maxfd = j;
        }
    }

    return 0;
}

int query_event(event_loop_t *loop, int fd)
{
    assert(loop != NULL);

    if (fd >= loop->setsize) return EV_NONE;
    
    return loop->events[fd].event;
}

int process_events(event_loop_t *loop)
{
    int j;
    int timeout;
    int event, fd;
    int processed, numevents;
    struct ev_event *ev;

    processed = 0;
    timeout = get_shortest_msec(loop->timers);
    
    numevents = poller_poll(loop, timeout);
    for (j = 0; j < numevents; j++) {

        ev = &loop->events[loop->actives[j].fd];
        event = loop->actives[j].event;
        fd = loop->actives[j].fd;

        /* 检查可能被修改的事件 */
        if (ev->event & event & EV_RDABLE) {
            ev->read(loop, fd, ev->data);
        }
        if (ev->event & event & EV_WRABLE) {
            ev->write(loop, fd, ev->data);
        }

        processed++;
    }

    processed += process_time_events(loop);
    
    return processed;
}

char *event_loop_poller_name()
{
    return poller_name();
}

void event_loop_start(event_loop_t *loop)
{
    loop->stop = 0;
    
    while (!loop->stop) {
        process_events(loop);
    }
}

void event_loop_stop(event_loop_t *loop)
{
    loop->stop = 1;
}
