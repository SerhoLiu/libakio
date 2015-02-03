#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <assert.h>
#include <time.h>
#include <sys/time.h>

#include "event.h"
#include "timer.h"


struct timer {
    int32_t      min_heap_idx;

    int64_t when_msec;

    void           *data;
    timer_callback *cb;

    struct timer *next;
};

struct timer_heap {
    int32_t      now, all;
    struct timer **tes;

    int maxsize;
    int allocs;
    int size;

    struct timer head;
    struct timer *tail;
};


struct timer_heap *timer_heap_create(int maxsize)
{
    struct timer_heap *heap;

    heap = malloc(sizeof(*heap));
    if (heap == NULL) {
        return NULL;
    }

    heap->tes = calloc(16, sizeof(struct timer *));
    if (heap->tes == NULL) {
        free(heap);
        return NULL;
    }
    heap->now = 0;
    heap->all = 16;

    heap->maxsize = maxsize < 16 ? 16 : maxsize;
    heap->size = heap->allocs = 0;
    heap->head.next = NULL;
    heap->tail = &heap->head;

    return heap;
}

void timer_heap_delete(struct timer_heap *heap)
{
    struct timer *curr, *next;
    
    free(heap->tes);

    for (curr = heap->head.next; curr != NULL; ) {
        next = curr->next;
        free(curr);
        curr = next;
    }

    free(heap);
}


static inline int _reserve(struct timer_heap *heap, int32_t n)
{
    int32_t      all;
    struct timer **p;

    all = heap->all * 2;
    if (all < n) all = n;
    
    p = realloc(heap->tes, all * sizeof(*p));
    if (p == NULL) return -1;
    heap->tes = p;
    heap->all = all;

    return 0;
}

static inline int _elem_greater(struct timer *a, struct timer *b)
{
    return a->when_msec > b->when_msec ? 1 : 0;
}

static inline void _shift_up(struct timer_heap *heap,
                             int32_t hole_index,
                             struct timer *t)
{
    int32_t parent;

    parent = (hole_index - 1) / 2;
    while (hole_index && _elem_greater(heap->tes[parent], t)) {
        heap->tes[hole_index] = heap->tes[parent];
        heap->tes[hole_index]->min_heap_idx = hole_index;
        hole_index = parent;
        parent = (hole_index - 1) / 2;
    }
    heap->tes[hole_index] = t;
    heap->tes[hole_index]->min_heap_idx = hole_index;
}

static inline void _shift_down(struct timer_heap *heap,
                               int32_t hole_index,
                               struct timer *t)
{
    int32_t min_child;

    min_child = 2 * (hole_index + 1);
    while (min_child <= heap->now) {
        min_child -= (min_child == heap->now || 
                      _elem_greater(heap->tes[min_child],
                                    heap->tes[min_child - 1]));

        if (!(_elem_greater(t, heap->tes[min_child]))) break;
        heap->tes[hole_index] = heap->tes[min_child];
        heap->tes[hole_index]->min_heap_idx = hole_index;
        hole_index = min_child;
        min_child = 2 * (hole_index + 1);
    }
    
    heap->tes[hole_index] = t;
    heap->tes[hole_index]->min_heap_idx = hole_index;
}

static inline struct timer *_heap_top(struct timer_heap *heap)
{ 
    return heap->now ? *heap->tes : NULL;
}

static inline int _heap_push(struct timer_heap *heap, struct timer *t)
{
    if (heap->all < heap->now+1 &&  _reserve(heap, heap->now+1)) {
        return -1;
    }
    _shift_up(heap, heap->now, t);

    heap->now++;
    
    return 0;
}

static inline int _heap_adjust(struct timer_heap *heap, struct timer *t)
{
    int32_t parent;

    parent = (t->min_heap_idx - 1) / 2;  
    if (t->min_heap_idx > 0 && _elem_greater(heap->tes[parent], t)) {
        _shift_up(heap, t->min_heap_idx, t);
    } else {
        _shift_down(heap, t->min_heap_idx, t);
    }
         
    return 0;
}

static inline int _heap_delete(struct timer_heap *heap, struct timer *t)
{
    int32_t      parent;
    struct timer *last;

    last = heap->tes[--heap->now];
    parent = (t->min_heap_idx - 1) / 2;
    
    if (t->min_heap_idx > 0 && _elem_greater(heap->tes[parent], last)) {
         _shift_up(heap, t->min_heap_idx, last);
    } else {
         _shift_down(heap, t->min_heap_idx, last);
    }
    t->min_heap_idx = -1;
    
    return 0;
}


static inline void get_now_time(int64_t *msec)
{
#ifdef USE_MONOTONIC

    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    *msec = ts.tv_sec * 1000 + ts.tv_nsec / (1000 * 1000);

#else

    struct timeval tv;
    gettimeofday(&tv, NULL);
    *msec = tv.tv_sec * 1000 + tv.tv_usec / 1000;

#endif
}

static inline void add_millisec_to_now(int32_t msec, int64_t *rmsec)
{
    int64_t cur_msec;

    get_now_time(&cur_msec);

    *rmsec = cur_msec + msec;
}


kz_timer_t *create_timer(event_loop_t *loop,
                         int msec,
                         timer_callback *cb,
                         void *data)
{
    struct timer      *te;
    struct timer_heap *heap;

    assert(loop != NULL);
    assert(msec > EV_TIMER_END);
    assert(cb != NULL);

    heap = loop->timers;
    if (heap->head.next != NULL) {
        te = heap->head.next;
        heap->head.next = te->next;
        if (heap->head.next == NULL) {
            heap->tail = &heap->head;
        }
        heap->size--;
    } else {
        te = malloc(sizeof(*te));
        if (te == NULL) {
            return NULL;
        }
        heap->allocs++;
    }

    te->next = NULL;
    te->min_heap_idx = -1;

    add_millisec_to_now(msec, &te->when_msec);
    
    te->cb = cb;
    te->data = data;

    return _heap_push(heap, te) == 0 ? te : NULL;
}

int delete_timer(event_loop_t *loop, kz_timer_t *t)
{
    struct timer_heap *heap;

    assert(loop != NULL);
    assert(t != NULL);

    if (t->min_heap_idx < 0 || t->next != NULL) {
        return -1;
    }

    heap = loop->timers;

    _heap_delete(heap, t);
    
    if (heap->allocs > heap->maxsize) {
        free(t);
        heap->allocs--;
    } else {
        heap->tail->next = t;
        heap->tail = t;
        heap->size++;
    }

    return 0;
}


int get_shortest_msec(struct timer_heap *heap)
{
    int64_t      now_msec, msec;
    struct timer *te;

    te = _heap_top(heap);
    if (te == NULL) {
        return -1;
    }

    get_now_time(&now_msec);

    msec = te->when_msec - now_msec;
    if (msec > INT32_MAX) {
        msec = INT32_MAX;
    }

    return msec < 0 ? 0 : msec;
}

int process_time_events(struct event_loop *loop)
{
    int     processed;
    int     retval;
    int64_t now_msec;

    struct timer       *te;
    struct timer_heap  *heap = loop->timers; 
    
    processed = 0;
    while ((te = _heap_top(heap)) != NULL) {
        
        get_now_time(&now_msec);
        if (now_msec < te->when_msec) {
            break;
        }

        retval = te->cb(loop, te->data);
        processed++;
        
        if (retval <= EV_TIMER_END) {
            delete_timer(loop, te);
        } else {
            add_millisec_to_now((int32_t)retval, &te->when_msec);
            _heap_adjust(heap, te);
        }
    }

    return processed;
}
