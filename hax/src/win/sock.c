#include "../common.h"


/*
 * local declarations
 */
static INIT_ONCE init = INIT_ONCE_STATIC_INIT;

static BOOL CALLBACK startup(PINIT_ONCE InitOnce, PVOID Parameter, PVOID *lpContext);

static void sockerr_proc(struct io_file_t file, void *arg);


/**
 * Create a socket.
 *   @fd: Ref. The file descriptor.
 *   @af: The address family.
 *   @type: The type.
 *   @prot: The protocol.
 *   &returns: Error.
 */
char *sys_socket(sys_fd_t *fd, int af, int type, int prot)
{
	InitOnceExecuteOnce(&init, startup, NULL, NULL);

	fd->sock = socket(af, type, 0);
	if(fd->sock == INVALID_SOCKET)
		return mprintf("Failed to create socket. %C.", sys_sockerr());

	fd->handle = WSACreateEvent();
	WSAEventSelect(fd->sock, fd->handle, FD_READ | FD_WRITE | FD_OOB | FD_ACCEPT | FD_CLOSE);

	return NULL;
}

/**
 * Connect a socket to a host and port.
 *   @sock: Ref. The output socket.
 *   @type: The socke type.
 *   @host: The host.
 *   @port: The port.
 *   &returns: Error.
 */
char *sys_connect(sys_fd_t *fd, int type, const char *host, uint16_t port)
{
	int err;
	char name[6];
	struct addrinfo hints, *res, *cur;

	InitOnceExecuteOnce(&init, startup, NULL, NULL);

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = type;
	hints.ai_flags = AI_PASSIVE;
	hints.ai_protocol = 0;
	hints.ai_canonname = NULL;
	hints.ai_addr = NULL;
	hints.ai_next = NULL;

	sprintf(name, "%u", port);
	err = getaddrinfo(host, name, &hints, &res);
	if(err != 0)
		return mprintf("Failed to connect to %s:%u.", host, port);

	for(cur = res; cur != NULL; cur = cur->ai_next) {
		fd->sock = socket(cur->ai_family, cur->ai_socktype, cur->ai_protocol);
		if(fd->sock < 0)
			continue;

		if(connect(fd->sock, cur->ai_addr, cur->ai_addrlen) == 0)
			break;

		close(fd->sock);
	}

	freeaddrinfo(res);

	if(cur == NULL)
		return mprintf("Failed to connect to %s:%u.", host, port);

	fd->handle = WSACreateEvent();
	WSAEventSelect(fd->sock, fd->handle, FD_CLOSE | FD_READ | FD_WRITE | FD_OOB | FD_ACCEPT);

	return NULL;
}

/**
 * Close a file descriptor.
 *   @sock: The descriptor.
 */
void sys_closesocket(sys_fd_t fd)
{
	closesocket(fd.sock);
	WSACloseEvent(fd.handle);
}

/**
 * Once callback for Winsock startup.
 *   @once: The init once.
 *   @param: Unused parameter.
 *   @ctx: Unused contxt.
 *   &returns: Always true.
 */
static BOOL CALLBACK startup(PINIT_ONCE once, PVOID param, PVOID *ctx)
{
	int err;
	WSADATA data;

	err = WSAStartup(MAKEWORD(2, 2), &data);
	if(err != 0)
		fatal("WSAStartup failed with error. %C.\n", sys_sockerr());

	return TRUE;
}


/**
 * Receive data on a socket.
 *   @fd: The file descriptor.
 *   @buf: The buffer.
 *   @nbyte: The number of bytes.
 *   @flags: The flags.
 *   &returns: The number of bytes read.
 */
size_t sys_recv(sys_fd_t fd, void *buf, size_t nbytes, int flags)
{
	int ret;

	WSAResetEvent(fd.handle);
	ret = recv(fd.sock, buf, nbytes, flags);
	if((ret == SOCKET_ERROR) && (WSAGetLastError() != WSAEWOULDBLOCK))
		fatal("Failed to read data on socket. %C.\n", sys_sockerr());

	return ret;
}

/**
 * Write data on a socket.
 *   @fd: The file descriptor.
 *   @buf: The buffer.
 *   @nbyte: The number of bytes.
 *   @flags: The flags.
 *   &returns: The number of bytes written.
 */
size_t sys_send(sys_fd_t fd, const void *buf, size_t nbytes, int flags)
{
	int ret;

	WSAResetEvent(fd.handle);
	ret = send(fd.sock, buf, nbytes, flags);
	if((ret == SOCKET_ERROR) && (WSAGetLastError() != WSAEWOULDBLOCK))
		fatal("Failed to write data to socket. %C.\n", sys_sockerr());

	return ret;
}


/**
 * Bind a socket.
 *   @fd: The file descriptor.
 *   @addr: The address.
 *   @len: The length.
 *   &returns: Error.
 */
char *sys_bind(sys_fd_t fd, const struct sockaddr *addr, int len)
{
	if(bind(fd.sock, addr, len) == SOCKET_ERROR)
		return mprintf("Failed to bind to socket. %C.", sys_sockerr());

	return NULL;
}

/**
 * Listen on a socket.
 *   @fd: The file descriptor.
 *   @backlog: The size of the backlog.
 *   &returns: Error.
 */
char *sys_listen(sys_fd_t fd, int backlog)
{
	if(listen(fd.sock, backlog) == SOCKET_ERROR)
		return mprintf("Failed to listen on socket. %C.", sys_sockerr());

	return NULL;
}

/**
 * Accpet a socket.
 *   @fd: The file descriptor.
 *   @client: Ref. The client socket.
 *   @addr: The address.
 *   @len: The length.
 *   &returns: Error.
 */
char *sys_accept(sys_fd_t fd, sys_fd_t *client, struct sockaddr *addr, int *len)
{
	WSAResetEvent(fd.handle);
	client->sock = accept(fd.sock, addr, len);
	if(client->sock == INVALID_SOCKET)
		return mprintf("Failed to accept a connection on socket. %C.", sys_sockerr());

	client->handle = WSACreateEvent();
	WSAEventSelect(client->sock, client->handle, FD_CLOSE | FD_READ | FD_WRITE | FD_OOB | FD_ACCEPT);

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
char *sys_setsockopt(sys_fd_t fd, int level, int opt, const void *val, int len)
{
	if(setsockopt(fd.sock, level, opt, val, len) == SOCKET_ERROR)
		return mprintf("Failed to set socket option. %C.", sys_sockerr());

	return NULL;
}


/**
 * Create a chunk for a socket error message.
 *   &returns: The chunk.
 */
struct io_chunk_t sys_sockerr(void)
{
	return (struct io_chunk_t){ sockerr_proc, NULL };
}
static void sockerr_proc(struct io_file_t file, void *arg)
{
	char str[1024];

	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, WSAGetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), str, sizeof(str), NULL);

	if(strlen(str) >= 3)
		str[strlen(str) - 3] = '\0';

	hprintf(file, "%s", str);
}
