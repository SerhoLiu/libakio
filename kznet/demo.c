/*
 * \file demo.h
 *
 * \brief A echo demo server
 *
 * Copyright (c) 2015 Serho Liu. All rights reserved.
 *
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>

#include "event.h"
#include "socket.h"
#include "buffer.h"


typedef struct {
    int eof;
    int writing;

    size_t total;
    buffer_chain_t *chain;

} connection_t;


static buffer_pool_t *pool;

static size_t prev_recv, prev_send;
static size_t recv_total, send_total;


int print_info(event_loop_t *loop, void *data)
{
    ssize_t recv = recv_total - prev_recv;
    ssize_t send = send_total - prev_send;

    prev_recv = recv_total;
    prev_send = send_total;

    float r = recv / 1024.0 / 1024;
    float s = send / 1024.0 / 1024;

    if (r != 0 || s != 0) {
        printf("\n");
        printf("recv %ldbyte\tsend %ldbyte\n", recv_total, send_total);
        printf("recv %0.3fMB/s\tsend %0.3fMB/s\n", r, s);
    }
    
    return 1000;
}

void client_writecb(event_loop_t *loop, int fd, void *data)
{
    int rv;
    connection_t *conn;

    conn = data;
    
    rv = buffer_chain_send(conn->chain, fd);
    if (rv < 0) {
        printf("[w]send error, close......\n");
        delete_event(loop, fd, EV_RDABLE|EV_WRABLE);
        close(fd);
        return;
    }
    conn->total -= conn->chain->sent;
    send_total += conn->chain->sent;

    if (conn->total == 0) {
        delete_event(loop, fd, EV_WRABLE);
        conn->writing = 0;
        if (conn->eof) {
            shutdown(fd, SHUT_WR);
        }
    }
}

void client_readcb(event_loop_t *loop, int fd, void *data)
{   
    int     rv;
    connection_t *conn;

    conn = data;

    rv = buffer_chain_recv(conn->chain, fd);
    if (rv < 0 || (rv == 0 && conn->total == 0)) {
        delete_event(loop, fd, EV_RDABLE|EV_WRABLE);
        close(fd);
        return;
    }
    if (rv == 0) {
        delete_event(loop, fd, EV_RDABLE);
        shutdown(fd, SHUT_RD);
        conn->eof = 1;
        return;
    }
    conn->total += conn->chain->recv;
    recv_total += conn->chain->recv;
    
    if (!conn->writing) {
        rv = buffer_chain_send(conn->chain, fd);
        if (rv < 0) {
            printf("[r]send error, close......\n");
            delete_event(loop, fd, EV_RDABLE|EV_WRABLE);
            close(fd);
            return;
        }
        conn->total -= conn->chain->sent;
        send_total += conn->chain->sent;

        if (conn->total > 0) {
            create_event(loop, fd, EV_WRABLE, client_writecb, conn);
            conn->writing = 1;
        } 
    }
}

void server_accept_cb(event_loop_t *loop, int fd, void *data)
{
    int  cfd;
    int  port;
    char ip[256];
    char err[SOCKET_ERR_LEN];

    while (1) {
        cfd = tcp_accept(err, fd, ip, 256, &port);
        if (cfd < 0) {
            if (errno == EAGAIN || errno == EINTR || errno == EWOULDBLOCK) {
                break;
            } else {
                printf("accept: %s\n", err);
                break;
            }
        }

        printf("client %s:%d\n", ip, port);
        if (set_nonblock(err, cfd) != 0) {
            printf("%s\n", err);
            close(cfd);
        }

        if (set_tcp_nodelay(err, cfd, 1) != 0) {
            printf("%s\n", err);
            close(cfd);
        }

        buffer_chain_t *chain = malloc(sizeof(*chain));
        assert(chain != NULL);
        chain->pool = pool;
        chain->rbuf = chain->wbuf = buffer_pool_get(pool);
        chain->sent = chain->recv = 0;
        
        connection_t *conn = malloc(sizeof(*conn));
        assert(conn != NULL);
        conn->chain = chain;
        conn->total = 0;
        conn->eof = 0;
        conn->writing = 0;

        create_event(loop, cfd, EV_RDABLE, client_readcb, conn);
    }
}

int main(int argc, char const *argv[])
{
    char err[SOCKET_ERR_LEN];

    pool = buffer_pool_create(100);
    prev_recv = prev_send = 0;
    recv_total = send_total = 0;

    int sfd = tcp_server_serve(err, "localhost", 3456, SOMAXCONN);
    if (sfd < 0) {
        printf("%d %s\n", sfd, err);
        return 0;
    }

    if (set_nonblock(err, sfd) != 0) {
        printf("%s\n", err);
        return 0;
    }

    if (set_tcp_nodelay(err, sfd, 1) != 0) {
        printf("%s\n", err);
        return 0;
    }

    event_loop_t *loop = event_loop_create(100, 10);
    assert(loop != NULL);

    create_timer(loop, 1000, print_info, NULL);
    create_event(loop, sfd, EV_RDABLE, server_accept_cb, NULL);

    event_loop_start(loop);
}
