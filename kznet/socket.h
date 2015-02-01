#ifndef _KZNET_SOCKET_H_
#define _KZNET_SOCKET_H_

#define SOCKET_ERR_LEN   256
#define CONNECT_BLOCK    0
#define CONNECT_NONBLOCK 1

int set_nonblock(char *err, int fd);
int set_close_on_exec(char *err, int fd);
int set_tcp_nodelay(char *err, int fd, int on);
int set_tcp_keepalive(char *err, int fd, int on);
int set_reuseaddr(char *err, int fd, int on);
int tcp_connect(char *err, char *addr, int port, int flags);
int tcp_accept(char *err, int sfd, char *ip, int ip_len, int *port);
int get_peer_name(int fd, char *ip, int ip_len, int *port);
int get_sock_name(int fd, char *ip, int ip_len, int *port);
int tcp_server_serve(char *err, char *bindaddr, int port, int backlog);

#endif