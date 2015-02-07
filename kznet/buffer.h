/**
 * \file buffer.h
 *
 * \brief Read & Write Buffer and Chain
 *
 * Copyright (c) 2015 Serho Liu. All rights reserved.
 *
 * Use of this source code is governed by a MIT-style license that can be
 * found in the MIT-LICENSE file.
 *
 */

#ifndef _KZNET_BUFFER_H_
#define _KZNET_BUFFER_H_

#include <sys/types.h>


typedef struct buffer buffer_t;
typedef struct buffer_chain buffer_chain_t;
typedef struct buffer_pool buffer_pool_t;


struct buffer {
    unsigned char *pos;    /*!< read marker */
    unsigned char *last;   /*!< write marker */
    unsigned char *start;  /*!< start of buffer(const) */
    unsigned char *end;    /*!< end of buffer(const) */

    struct buffer *next;   /*!< next buffer */
};

struct buffer_chain {
    buffer_t *rbuf;
    buffer_t *wbuf;

    size_t sent;
    size_t recv;

    buffer_pool_t *pool;
};


#define SM_MAX_IOV   8
#define BUFFER_SIZE  4096
#define BUFFER_HSIZE (sizeof(struct buffer))


static inline size_t buffer_rsize(buffer_t *buf)
{
    return (size_t)(buf->last - buf->pos);
}

static inline size_t buffer_wsize(buffer_t *buf)
{
    return (size_t)(buf->end - buf->last);
}

buffer_pool_t *buffer_pool_create(int maxsize);

void buffer_pool_delete(buffer_pool_t *pool);

buffer_t *buffer_pool_get(buffer_pool_t *pool);

int buffer_pool_put(buffer_pool_t *pool, buffer_t *buf);

buffer_t *shrink_chain(buffer_chain_t *chain, size_t size);

int buffer_chain_recv(buffer_chain_t *chain, int fd);
int buffer_chain_send(buffer_chain_t *chain, int fd);

#endif
