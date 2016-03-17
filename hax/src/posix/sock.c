#include "../common.h"
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>


/*
 * global declarations
 */
sys_sock_t sys_badsock = -1;


/**
 * Create a socket.
 *   @sock: Ref. The socket.
 *   @af: The address family.
 *   @type: The type.
 *   @prot: The protocol.
 *   &returns: Error.
 */
char *sys_socket(sys_sock_t *sock, int af, int type, int prot)
{
	*sock = socket(af, type, 0);
	if(*sock < 0)
		return mprintf("Failed to create socket. %s.", strerror(errno));

	return NULL;
}

/**
 * Close a socket.
 *   @sock: The socket.
 */
void sys_closesocket(sys_sock_t sock)
{
	close(sock);
}


/**
 * Receive data on a socket.
 *   @sock: The socket.
 *   @buf: The buffer.
 *   @nbyte: The number of bytes.
 *   @flags: The flags.
 *   &returns: The number of bytes read.
 */
size_t sys_recv(sys_sock_t sock, void *buf, size_t nbytes, int flags)
{
	ssize_t ret;

	do
		ret = recv(sock, buf, nbytes, flags);
	while((ret < 0) && (errno == EINTR));

	if(ret < 0) {
		if((errno != EAGAIN) && (errno != EWOULDBLOCK))
			fatal("Failed to read data on socket. %s.", strerror(errno));
		else
			return 0;
	}

	return ret;
}

/**
 * Write data on a socket.
 *   @sock: The socket.
 *   @buf: The buffer.
 *   @nbyte: The number of bytes.
 *   @flags: The flags.
 *   &returns: The number of bytes written.
 */
size_t sys_send(sys_sock_t sock, const void *buf, size_t nbytes, int flags)
{
	ssize_t ret;

	do
		ret = send(sock, buf, nbytes, flags);
	while((ret < 0) && (errno == EINTR));

	if(ret < 0) {
		if((errno != EAGAIN) && (errno != EWOULDBLOCK))
			fatal("Failed to write data on socket. %s.", strerror(errno));
		else
			return 0;
	}

	return ret;
}


/**
 * Bind a socket.
 *   @sock: The socket.
 *   @addr: The address.
 *   @len: The length.
 *   &returns: Error.
 */
char *sys_bind(sys_sock_t sock, const struct sockaddr *addr, socklen_t len)
{
	if(bind(sock, addr, len) < 0)
		return mprintf("Failed to set socket option. %s.", strerror(errno));

	return NULL;
}

/**
 * Listen on a socket.
 *   @sock: The socket.
 *   @backlog: The size of the backlog.
 *   &returns: Error.
 */
char *sys_listen(sys_sock_t sock, int backlog)
{
	if(listen(sock, backlog) < 0)
		return mprintf("Failed to set socket option. %s.", strerror(errno));

	return NULL;
}

/**
 * Accpet a socket.
 *   @sock: The socket.
 *   @ref: The client socket.
 *   @addr: The address.
 *   @len: The length.
 *   &returns: Error.
 */
char *sys_accept(sys_sock_t sock, sys_sock_t *client, struct sockaddr *addr, socklen_t *len)
{
	*client = accept(sock, addr, len);
	if(*client < 0)
		return mprintf("Failed to set socket option. %s.", strerror(errno));

	return NULL;
}

/**
 * Set socket option.
 *   @sock: The socket.
 *   @level: The protocol level.
 *   @opt: The option.
 *   @val: The value.
 *   @len: The value length.
 *   &returns: Error.
 */
char *sys_setsockopt(sys_sock_t sock, int level, int opt, const void *val, socklen_t len)
{
	if(setsockopt(sock, level, opt, val, len) < 0)
		return mprintf("Failed to set socket option. %s.", strerror(errno));

	return NULL;
}
