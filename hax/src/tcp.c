#include "common.h"


/*
 * default defintions
 */
#define DEFSIZE	(16*1024)


/**
 * Client structure.
 *   @fd: The file descriptor.
 *   @defsize: The default read size.
 *   @in, out: The input and output data lists.
 *   @events: The pending events.
 */
struct tcp_client_t {
	sys_fd_t fd;

	size_t defsize;
	struct data_t *in, *out;

	enum sys_poll_e events;
};

/**
 * Server structure.
 *   @fd: The file descriptor.
 */
struct tcp_server_t {
	sys_fd_t fd;
};

/**
 * Data structure.
 *   @next: The next data.
 *   @idx, len: The index length.
 *   @buf: The buffer.
 */
struct data_t {
	struct data_t *next;

	size_t idx, len;
	uint8_t buf[];
};


/**
 * Create a client from a file descriptor.
 *   @fd: The file descriptor.
 *   &returns: The client.
 */
struct tcp_client_t *tcp_client_new(sys_fd_t fd)
{
	struct tcp_client_t *client;

	client = malloc(sizeof(struct tcp_client_t));
	*client = (struct tcp_client_t){ fd, 16*1024, NULL, NULL, 0 };

	return client;
}

/**
 * Delete a client.
 *   @client: The client.
 */
void tcp_client_delete(struct tcp_client_t *client)
{
	struct data_t *cur, *next;

	for(cur = client->in; cur != NULL; cur = next) {
		next = cur->next;
		free(cur);
	}

	for(cur = client->out; cur != NULL; cur = next) {
		next = cur->next;
		free(cur);
	}

	free(client);
}


/**
 * Open a connection to a server.
 *   @client: Ref. The client.
 *   @host: The host.
 *   @port: The port.
 *   &returns: Error.
 */
char *tcp_client_open(struct tcp_client_t **client, const char *host, uint16_t port)
{
#define onexit
	sys_fd_t fd;

	chkfail(sys_connect(&fd, SOCK_STREAM, host, port));
	*client = tcp_client_new(fd);

	return NULL;
#undef onexit
}

/**
 * Close a connection.
 *   @client: The client.
 */
void tcp_client_close(struct tcp_client_t *client)
{
	sys_closesocket(client->fd);
	tcp_client_delete(client);
}


/**
 * Retrieve the file descriptor of the client.
 *   @client: The client.
 *   &returns: The file descriptor.
 */
sys_fd_t tcp_client_fd(struct tcp_client_t *client)
{
	return client->fd;
}

/**
 * Retrieve the file descriptor of the client.
 *   @client: The client.
 *   &returns: The file descriptor.
 */
struct sys_poll_t tcp_client_poll(struct tcp_client_t *client)
{
	return sys_poll_fd(client->fd, sys_poll_in_e | sys_poll_err_e | (client->out ? sys_poll_out_e : 0));
}

/**
 * Retrieve the number of available bytes from a client.
 *   @client: The client.
 *   &returns: The number of bytes.
 */
size_t tcp_client_avail(struct tcp_client_t *client)
{
	struct data_t *data;
	size_t nbytes = 0;

	for(data = client->in; data != NULL; data = data->next)
		nbytes += data->len - data->idx;

	return nbytes;
}

/**
 * Retrieve the number of queued bytes on a client.
 *   @client: The client.
 *   &returns: The number of bytes.
 */
size_t tcp_client_queue(struct tcp_client_t *client)
{
	struct data_t *data;
	size_t nbytes = 0;

	for(data = client->out; data != NULL; data = data->next)
		nbytes += data->len - data->idx;

	return nbytes;
}


/**
 * Attempt to read from the TCP connection.
 *   @client: The client.
 *   @buf: The buffer.
 *   @nbytes: The number of bytes to read.
 *   &returns: True if read, false if the request is pending.
 */
bool tcp_client_read(struct tcp_client_t *client, void *restrict buf, size_t nbytes)
{
	size_t len;
	struct data_t *data;

	if(tcp_client_avail(client) < nbytes) {
		client->events |= sys_poll_in_e;
		return false;
	}

	while(nbytes > 0) {
		data = client->in;
		if(nbytes < (data->len - data->idx))
			len = nbytes;
		else
			len = data->len - data->idx;

		memcpy(buf, data->buf + data->idx, len);

		nbytes -= len;
		data->idx += len;

		if(data->idx != data->len)
			break;

		client->in = data->next;
		free(data);
	}

	return true;
}

/**
 * Write data to a TCP connection.
 *   @client: The client.
 *   @buf: The buffer.
 *   @nbytes: The number of bytes.
 */
void tcp_client_write(struct tcp_client_t *client, const void *restrict buf, size_t nbytes)
{
	struct data_t **data;

	data = &client->out;
	while(*data != NULL)
		data = &(*data)->next;

	*data = malloc(sizeof(struct data_t) + nbytes);
	(*data)->idx = 0;
	(*data)->len = nbytes;
	(*data)->next = NULL;
	memcpy((*data)->buf, buf, nbytes);

	client->events |= sys_poll_out_e;
}

/**
 * Process data on a client.
 *   @client: The client.
 *   @events: The events.
 *   &returns: The success flag.
 */
bool tcp_client_proc(struct tcp_client_t *client, enum sys_poll_e events)
{
	if(events & sys_poll_in_e) {
		ssize_t ret;
		struct data_t **ref, *data;

		data = malloc(sizeof(struct data_t) + DEFSIZE);

		ret = sys_recv(client->fd, data->buf, DEFSIZE, 0);
		if(ret <= 0) {
			free(data);
			return false;
		}

		data->next = NULL;
		data->idx = 0;
		data->len = ret;

		ref = &client->in;
		while(*ref != NULL)
			ref = &(*ref)->next;

		data = realloc(data, sizeof(struct data_t) + data->len);
		*ref = data;

		client->events &= ~sys_poll_in_e;
	}

	if(events & sys_poll_out_e) {
		struct data_t *data;

		while(client->out != NULL) {
			ssize_t ret;

			data = client->out;
			ret = sys_send(client->fd, data->buf + data->idx, data->len - data->idx, 0);
			if(ret <= 0)
				return false;

			data->idx += ret;
			if(data->idx != data->len)
				break;

			client->out = data->next;
			free(data);
		}

		client->events &= ~sys_poll_out_e;
	}

	return true;
}


/**
 * Open a TCP server.
 *   @server: Ref. The server.
 *   @port: The port.
 *   &returns: Error.
 */
char *tcp_server_open(struct tcp_server_t **server, uint16_t port)
{
#define onexit if(sys_isfd(fd)) sys_closesocket(fd);
	int val;
	struct sockaddr_in addr;
	sys_fd_t fd = sys_badfd;

	chkfail(sys_socket(&fd, AF_INET, SOCK_STREAM, 0));

	val = 1;
	chkfail(sys_setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(int)));

	memset(&addr, 0x00, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);

	chkfail(sys_bind(fd, (struct sockaddr *)&addr, sizeof(addr)));

	*server = malloc(sizeof(struct tcp_server_t));
	(*server)->fd = fd;

	return NULL;
#undef onexit
}

/**
 * Close a server.
 *   @server: The server.
 */
void tcp_server_close(struct tcp_server_t *server)
{
	sys_closesocket(server->fd);
	free(server);
}


/**
 * Retrieve the server file descriptor.
 *   @server: The server.
 *   &returns: The file descriptor.
 */
sys_fd_t tcp_server_fd(struct tcp_server_t *server)
{
	return server->fd;
}

/**
 * Retrieve the poll information from a server.
 *   @server: The server.
 *   &returns: The poll information.
 */
struct sys_poll_t tcp_server_poll(struct tcp_server_t *server)
{
	return sys_poll_fd(server->fd, sys_poll_in_e);
}


/**
 * Listen on a server.
 *   @server: The server.
 *   &returns: Error.
 */
char *tcp_server_listen(struct tcp_server_t *server)
{
#define onexit
	chkfail(sys_listen(server->fd, SOMAXCONN));
	
	return NULL;
#undef onexit
}

/**
 * Accept a new connection from the server.
 *   @server: The server.
 *   @fd: Ref. The output file descriptor.
 *   &returns: Error.
 */
char *tcp_server_accept(struct tcp_server_t *server, sys_fd_t *fd)
{
#define onexit
	socklen_t size;
	struct sockaddr_in addr;

	size = sizeof(addr);
	chkfail(sys_accept(server->fd, fd, (struct sockaddr *)&addr, &size));

	return NULL;
#undef onexit
}
