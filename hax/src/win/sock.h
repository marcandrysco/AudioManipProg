#ifndef SOCK_H
#define SOCK_H

/*
 * socket declarations
 */
char *sys_socket(sys_fd_t *fd, int af, int type, int prot);
char *sys_connect(sys_fd_t *fd, int type, const char *host, uint16_t port);
void sys_closesocket(sys_fd_t fd);

size_t sys_recv(sys_fd_t fd, void *buf, size_t nbytes, int flags);
size_t sys_send(sys_fd_t fd, const void *buf, size_t nbytes, int flags);

char *sys_bind(sys_fd_t fd, const struct sockaddr *addr, int len);
char *sys_listen(sys_fd_t fd, int backlog);
char *sys_accept(sys_fd_t fd, sys_fd_t *client, struct sockaddr *addr, int *len);
char *sys_setsockopt(sys_fd_t fd, int level, int opt, const void *val, int len);

struct io_chunk_t sys_sockerr(void);

#endif
