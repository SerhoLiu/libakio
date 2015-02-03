#ifndef _KZNET_EVENT_H_
#define _KZNET_EVENT_H_


#define EV_NONE      0
#define EV_RDABLE    1
#define EV_WRABLE    2
#define EV_TIMER_END 0

typedef struct timer kz_timer_t;
typedef struct event_loop event_loop_t;

typedef int timer_callback(event_loop_t *loop, void *data);
typedef void event_callback(event_loop_t *loop, int fd, void *data);


struct event_loop {
    int maxfd;
    int setsize;

    struct ev_event  *events;
    struct ev_active *actives;

    struct poller     *poller;
    struct timer_heap *timers;

    int  stop;
} event_loop;


int create_event(event_loop_t *loop,
                 int fd,
                 int event,
                 event_callback *cb,
                 void *data);

int delete_event(event_loop_t *loop, int fd, int event);

int query_event(event_loop_t *loop, int fd);

kz_timer_t *create_timer(event_loop_t *loop,
                         int msec,
                         timer_callback *cb,
                         void *data);

int delete_timer(event_loop_t *loop, kz_timer_t *t);

event_loop_t *event_loop_create(int maxevents, int maxtimers);

void event_loop_delete(event_loop_t *loop);
char *event_loop_poller_name();
void event_loop_start(event_loop_t *loop);
void event_loop_stop(event_loop_t *loop);

#endif