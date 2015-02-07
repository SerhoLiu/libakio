/*
 * Copyright (c) 2015 Serho Liu. All rights reserved.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <sys/uio.h>

#include "buffer.h"


struct buffer_pool {
    int maxsize;
    int allocs;
    int size;

    struct buffer  head;
    struct buffer *tail;
};


buffer_pool_t *buffer_pool_create(int maxsize)
{
    buffer_pool_t *pool;

    pool = malloc(sizeof(*pool));
    if (pool == NULL) {
        return NULL;
    }

    pool->maxsize = maxsize < 32 ? 32 : maxsize;
    pool->size = pool->allocs = 0;
    pool->head.next = NULL;
    pool->tail = &pool->head;

    return pool;
}

void buffer_pool_delete(buffer_pool_t *pool)
{
    buffer_t *curr, *next;

    assert(pool != NULL);

    for (curr = pool->head.next; curr != NULL; ) {
        next = curr->next;
        free(curr);
        curr = next;
    }
    free(pool);
}

static buffer_t *buffer_pool_peek(buffer_pool_t *pool)
{
    buffer_t *buf;

    assert(pool != NULL);

    if (pool->head.next != NULL) {
        return pool->head.next;
    }

    buf = malloc(BUFFER_SIZE);
    if (buf == NULL) return NULL;
    
    buf->next = NULL;
    buf->start = (unsigned char *)buf + BUFFER_HSIZE;
    buf->pos = buf->last = buf->start;
    buf->end = (unsigned char *)buf + BUFFER_SIZE;

    pool->head.next = pool->tail = buf;
    pool->size++;
    pool->allocs++;

    return buf;
}

static inline void buffer_detach(buffer_pool_t *pool, buffer_t *buf)
{
    pool->head.next = buf->next;
    buf->next = NULL;
    if (pool->head.next == NULL) {
        pool->tail = &pool->head;
    }
    pool->size--;
}

buffer_t *buffer_pool_get(buffer_pool_t *pool)
{
    buffer_t *buf;

    assert(pool != NULL);

    buf = buffer_pool_peek(pool);
    if (buf == NULL) return NULL;

    buffer_detach(pool, buf);

    return buf;
}

int buffer_pool_put(buffer_pool_t *pool, buffer_t *buf)
{
    assert(pool != NULL);
    assert(buf != NULL);

    if (pool->allocs > pool->maxsize) {
        free(buf);
        pool->allocs--;
        return 0;
    }

    buf->next = NULL;
    buf->pos = buf->last = buf->start;

    pool->tail->next = buf;
    pool->tail = buf;
    pool->size++;

    return 0;
}

buffer_t *shrink_chain(buffer_chain_t *chain, size_t size)
{
    size_t   rlen;
    buffer_t *buf;

    while (chain->rbuf != chain->wbuf) {
            
        buf = chain->rbuf;
        rlen = buffer_rsize(buf);
        
        if (size >= rlen) {
            size -= rlen;
            chain->rbuf = buf->next;
            buffer_pool_put(chain->pool, buf);
            continue;
        }
        
        buf->pos += size;
        
        return buf;
    }

    buf = chain->rbuf;
    assert(size <= buffer_rsize(buf));

    buf->pos += size;
    if (buf->pos == buf->last) {
        buf->pos = buf->last = buf->start;
    }

    return buf;
}

int buffer_chain_recv(buffer_chain_t *chain, int fd)
{
    size_t   wlen, tlen;
    ssize_t  rv;
    buffer_t *wbuf;
    buffer_t *temp;
    buffer_pool_t *pool;
    struct iovec vec[2];

    chain->recv = 0;
    pool = chain->pool;

    while (1) {

        wbuf = chain->wbuf;
        wlen = buffer_wsize(wbuf);
        temp = buffer_pool_peek(pool);
        tlen = buffer_wsize(temp);

        vec[0].iov_len = wlen;
        vec[0].iov_base = wbuf->last;
        vec[1].iov_len = tlen;
        vec[1].iov_base = temp->last;

        rv = readv(fd, vec, 2);

        if (rv == 0) return 0;
        if (rv == -1) {
            if (errno == EAGAIN) {
                return 1;
            }
            if (errno != EINTR) {
                return -1;
            }

            continue;
        }

        chain->recv += rv;

        if (rv <= wlen) {
            wbuf->last += rv;
            break;
        }

        wbuf->last += wlen;
        buffer_detach(pool, temp);
        temp->last = temp->pos + (rv - wlen);
        chain->wbuf->next = temp;
        chain->wbuf = temp;

        if (rv != wlen + tlen) {
            break;
        }
    }

    return 1;
}

int buffer_chain_send(buffer_chain_t *chain, int fd)
{
    int      i, complete;
    size_t   rlen, total;
    ssize_t  rv;
    buffer_t *buf;

    struct iovec   vec[SM_MAX_IOV];

    chain->sent = 0;
    
    while (1) {
        
        complete = 0;
        total = 0;

        i = 0;
        for (buf = chain->rbuf; buf != NULL; buf = buf->next) {
            vec[i].iov_base = buf->pos;
            rlen = buffer_rsize(buf);
            vec[i].iov_len = rlen;
            
            total += rlen;
            
            i++;
            if (i == SM_MAX_IOV) {
                break;
            }
        }

        rv = writev(fd, vec, i);

        if (rv == 0) return 0;
        if (rv == -1) {
            if (errno == EAGAIN) {
                return 1;
            }
            if (errno != EINTR) {
                return -1;
            }

            continue;
        }

        chain->sent += rv;

        if (rv == total) {
            complete = 1;
        }

        buf = shrink_chain(chain, rv);
        if (!complete || buf->pos == buf->last) {
            break;
        }
    }

    return 1;
}
