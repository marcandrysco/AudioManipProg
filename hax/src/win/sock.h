#ifndef SOCK_H
#define SOCK_H

/*
 * socket declarations
 */
extern sys_sock_t sys_badsock;

char *sys_socket(sys_sock_t *sock, int af, int type, int prot);
void sys_closesocket(sys_sock_t sock);

size_t sys_recv(sys_sock_t sock, void *buf, size_t nbytes, int flags);
size_t sys_send(sys_sock_t sock, const void *buf, size_t nbytes, int flags);

char *sys_bind(sys_sock_t sock, const struct sockaddr *addr, int len);
char *sys_listen(sys_sock_t sock, int backlog);
char *sys_accept(sys_sock_t sock, sys_sock_t *client, struct sockaddr *addr, int *len);
char *sys_setsockopt(sys_sock_t sock, int level, int opt, const void *val, int len);

struct io_chunk_t sys_sockerr(void);


/**
 * Check if a file descriptor is valid.
 *   @sock: The socket.
 *   &returns: True if valid, false otherwise.
 */
static inline bool sys_issock(sys_sock_t sock)
{
	return sock != INVALID_SOCKET;
}

#endif
