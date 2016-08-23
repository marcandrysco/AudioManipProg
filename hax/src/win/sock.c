#include "../common.h"


/*
 * local declarations
 */
static INIT_ONCE init = INIT_ONCE_STATIC_INIT;

static BOOL CALLBACK startup(PINIT_ONCE InitOnce, PVOID Parameter, PVOID *lpContext);

static void sockerr_proc(struct io_file_t file, void *arg);

/*
 * global declarations
 */
sys_sock_t sys_badsock = INVALID_SOCKET;


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
	InitOnceExecuteOnce(&init, startup, NULL, NULL);

	*sock = socket(af, type, 0);
	if(*sock == INVALID_SOCKET)
		return mprintf("Failed to create socket. %C.", sys_sockerr());

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
char *sys_connect(sys_sock_t *sock, int type, const char *host, uint16_t port)
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
		*sock = socket(cur->ai_family, cur->ai_socktype, cur->ai_protocol);
		if(*sock < 0)
			continue;

		if(connect(*sock, cur->ai_addr, cur->ai_addrlen) == 0)
			break;

		close(*sock);
	}

	freeaddrinfo(res);

	if(cur == NULL)
		return mprintf("Failed to connect to %s:%u.", host, port);

	return NULL;
}

/**
 * Close a file descriptor.
 *   @sock: The descriptor.
 */
void sys_closesocket(sys_sock_t sock)
{
	closesocket(sock);
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
 *   @sock: The socket.
 *   @buf: The buffer.
 *   @nbyte: The number of bytes.
 *   @flags: The flags.
 *   &returns: The number of bytes read.
 */
size_t sys_recv(sys_sock_t sock, void *buf, size_t nbytes, int flags)
{
	int ret;

	do
		ret = recv(sock, buf, nbytes, flags);
	while((ret < 0) && (errno == EINTR));

	if(ret == SOCKET_ERROR)
		fatal("Failed to read data on socket. %C.\n", sys_sockerr());

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
	int ret;

	do
		ret = send(sock, buf, nbytes, flags);
	while((ret < 0) && (errno == EINTR));

	if(ret == SOCKET_ERROR)
		fatal("Failed to write data to socket. %C.\n", sys_sockerr());

	return ret;
}


/**
 * Bind a socket.
 *   @sock: The socket.
 *   @addr: The address.
 *   @len: The length.
 *   &returns: Error.
 */
char *sys_bind(sys_sock_t sock, const struct sockaddr *addr, int len)
{
	if(bind(sock, addr, len) == SOCKET_ERROR)
		return mprintf("Failed to set socket option. %C.", sys_sockerr());

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
	if(listen(sock, backlog) == SOCKET_ERROR)
		return mprintf("Failed to set socket option. %C.", sys_sockerr());

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
char *sys_accept(sys_sock_t sock, sys_sock_t *client, struct sockaddr *addr, int *len)
{
	*client = accept(sock, addr, len);
	if(*client == INVALID_SOCKET)
		return mprintf("Failed to set socket option. %C.", sys_sockerr());

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
char *sys_setsockopt(sys_sock_t sock, int level, int opt, const void *val, int len)
{
	if(setsockopt(sock, level, opt, val, len) == SOCKET_ERROR)
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
	hprintf(file, "%s", str);
}
