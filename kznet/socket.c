#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

#include "socket.h"


static void socket_error(char *err, const char *fmt, ...)
{
    va_list ap;

    if (err == NULL) return;
    va_start(ap, fmt);
    vsnprintf(err, SOCKET_ERR_LEN, fmt, ap);
    va_end(ap);
}

int set_nonblock(char *err, int fd)
{
    int flags;

    if ((flags = fcntl(fd, F_GETFL)) == -1) {
        socket_error(err, "fcntl(F_GETFL): %s", strerror(errno));
        return -1;
    }
    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1) {
        socket_error(err, "fcntl(F_SETFL,O_NONBLOCK): %s", strerror(errno));
        return -1;
    }

    return 0;
}

int set_close_on_exec(char *err, int fd)
{
    int flags;

    if ((flags = fcntl(fd, F_GETFD, 0)) == -1) {
        socket_error(err, "fcntl(F_GETFD): %s", strerror(errno));
        return -1;
    }
    if (fcntl(fd, F_SETFD, flags | FD_CLOEXEC) == -1) {
        socket_error(err, "fcntl(F_SETFD,FD_CLOEXEC): %s", strerror(errno));
        return -1;
    }

    return 0;
}

int set_tcp_nodelay(char *err, int fd, int on)
{
    if (setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &on, sizeof(on)) == -1) {
        socket_error(err, "setsockopt TCP_NODELAY: %s", strerror(errno));
        return -1;
    }

    return 0;
}

int set_tcp_keepalive(char *err, int fd, int on)
{
    if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &on, sizeof(on)) == -1) {
        socket_error(err, "setsockopt SO_KEEPALIVE: %s", strerror(errno));
        return -1;
    }

    return 0;
}

int set_reuseaddr(char *err, int fd, int on)
{
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1) {
        socket_error(err, "setsockopt SO_REUSEADDR: %s", strerror(errno));
        return -1;
    }

    return 0;
}

int tcp_connect(char *err, char *addr, int port, int flags)
{
    int s, rv;
    char s_port[sizeof("65535")];
    struct addrinfo hints, *servinfo, *p;

    s = -1;
    snprintf(s_port, sizeof(s_port), "%d", port);
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    if ((rv = getaddrinfo(addr, s_port, &hints, &servinfo)) != 0) {
        socket_error(err, "%s", gai_strerror(rv));
        return -1;
    }
    for (p = servinfo; p != NULL; p = p->ai_next) {
        /* Try to create the socket and to connect it.
         * If we fail in the socket() call, or on connect(), we retry with
         * the next entry in servinfo.
         */
        if ((s = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
            continue;
        if (set_reuseaddr(err, s, 1) != 0) goto error;
        if (flags & CONNECT_NONBLOCK && set_nonblock(err,s) != 0)
            goto error;
        if (connect(s, p->ai_addr, p->ai_addrlen) == -1) {
            /* If the socket is non-blocking, it is ok for connect() to
             * return an EINPROGRESS error here.
             */
            if (errno == EINPROGRESS && flags & CONNECT_NONBLOCK)
                goto end;
            close(s);
            s = -1;
            continue;
        }

        /* If we ended an iteration of the for loop without errors, we
         * have a connected socket. Let's return to the caller.
         */
        goto end;
    }
    if (p == NULL)
        socket_error(err, "creating socket: %s", strerror(errno));

error:
    if (s != -1) {
        close(s);
        s = -1;
    }
end:
    freeaddrinfo(servinfo);
    return s;
}

static inline void sockaddr_tostring(struct sockaddr_storage *sa,
        char *ip, int ip_len, int *port)
{
    struct sockaddr_in *s;
    struct sockaddr_in6 *s6;

    if (sa->ss_family == AF_INET) {
        s = (struct sockaddr_in *)sa;
        if (ip != NULL) {
            inet_ntop(AF_INET, (void *)&(s->sin_addr), ip, ip_len);
        }
        if (port != NULL) {
            *port = ntohs(s->sin_port);
        }
    } else {
        s6 = (struct sockaddr_in6 *)sa;
        if (ip != NULL) {
            inet_ntop(AF_INET6, (void *)&(s6->sin6_addr), ip, ip_len);
        }
        if (port != NULL) {
            *port = ntohs(s6->sin6_port);
        }
    }
}

int tcp_accept(char *err, int sfd, char *ip, int ip_len, int *port)
{
    int fd;
    struct sockaddr_storage sa;
    
    socklen_t salen = sizeof(sa);
    
    fd = accept(sfd, (struct sockaddr *)&sa, &salen);
    if (fd != -1) {
        sockaddr_tostring(&sa, ip, ip_len, port);
    }
    
    return fd;
}

int get_peer_name(int fd, char *ip, int ip_len, int *port)
{
    struct sockaddr_storage sa;
    socklen_t salen = sizeof(sa);

    if (getpeername(fd, (struct sockaddr *)&sa, &salen) == -1) {
        if (port != NULL) *port = 0;
        if (ip != NULL) {
            ip[0] = '?';
            ip[1] = '\0';
        }
        return -1;
    }
    sockaddr_tostring(&sa, ip, ip_len, port);

    return 0;
}

int get_sock_name(int fd, char *ip, int ip_len, int *port)
{
    struct sockaddr_storage sa;
    socklen_t salen = sizeof(sa);

    if (getsockname(fd, (struct sockaddr *)&sa, &salen) == -1) {
        if (port != NULL) *port = 0;
        if (ip != NULL) {
            ip[0] = '?';
            ip[1] = '\0';
        }
        return -1;
    }
    sockaddr_tostring(&sa, ip, ip_len, port);

    return 0;
}

int tcp_server_serve(char *err, char *addr, int port, int backlog)
{
    int sfd, rv;
    char s_port[sizeof("65535")];
    struct addrinfo hints, *servinfo, *p;

    snprintf(s_port, 6, "%d", port);
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((rv = getaddrinfo(addr, s_port, &hints, &servinfo)) != 0) {
        socket_error(err, "%s", gai_strerror(rv));
        return -1;
    }
    for (p = servinfo; p != NULL; p = p->ai_next) {
        if ((sfd = socket(p->ai_family, p->ai_socktype,
                    p->ai_protocol)) == -1) {
            continue;
        }

        if (set_reuseaddr(err, sfd, 1) != 0) {
            goto error;
        }

        if (bind(sfd, p->ai_addr, p->ai_addrlen) != 0) {
            socket_error(err, "bind: %s", strerror(errno));
            close(sfd);
            goto error;
        }

        if (listen(sfd, backlog) != 0) {
            socket_error(err, "listen: %s", strerror(errno));
            close(sfd);
            goto error;
        }
        goto end;
    }
    if (p == NULL) {
        socket_error(err, "unable to bind socket");
        goto error;
    }

error:
    sfd = -1;
end:
    freeaddrinfo(servinfo);
    return sfd;
}
