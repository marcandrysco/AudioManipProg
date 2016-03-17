#include "common.h"


/*
 * default defintions
 */
#define DEFSIZE	(16*1024)


/**
 * Client structure.
 *   @sock: The socket.
 *   @defsize: The default read size.
 *   @in, out: The input and output data lists.
 *   @events: The pending events.
 */
struct tcp_client_t {
	sys_sock_t sock;

	size_t defsize;
	struct data_t *in, *out;

	enum sys_poll_e events;
};

/**
 * Server structure.
 *   @sock: The socket.
 */
struct tcp_server_t {
	sys_sock_t sock;
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
 * Create a client from a socket.
 *   @client: Ref. The client.
 *   @sock: The socket.
 */
struct tcp_client_t *tcp_client_new(sys_sock_t sock)
{
	struct tcp_client_t *client;

	client = malloc(sizeof(struct tcp_client_t));
	*client = (struct tcp_client_t){ sock, 16*1024, NULL, NULL, 0 };

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
 * Retrieve the file descriptor of the client.
 *   @client: The client.
 *   &returns: The file descriptor.
 */
sys_sock_t tcp_client_sock(struct tcp_client_t *client)
{
	return client->sock;
}

/**
 * Retrieve the file descriptor of the client.
 *   @client: The client.
 *   &returns: The file descriptor.
 */
struct sys_poll_t tcp_client_poll(struct tcp_client_t *client)
{
	return sys_poll_sock(client->sock, client->events);
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
	memcpy((*data)->buf, buf, nbytes);

	client->events |= sys_poll_out_e;
}

/**
 * Process data on a client.
 *   @client: The client.
 *   @events: The events.
 */
void tcp_client_proc(struct tcp_client_t *client, enum sys_poll_e events)
{
	if(events & sys_poll_in_e) {
		struct data_t **ref, *data;

		data = malloc(sizeof(struct data_t) + DEFSIZE);
		data->next = NULL;
		data->idx = 0;
		data->len = sys_recv(client->sock, data->buf, DEFSIZE, 0);

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
			data = client->out;
			data->idx += sys_send(client->sock, data->buf + data->idx, data->len - data->idx, 0);
			if(data->idx != data->len)
				break;

			client->out = data->next;
			free(data);
		}

		client->events &= ~sys_poll_out_e;
	}
}


/**
 * Open a TCP server.
 *   @server: Ref. The server.
 *   @port: The port.
 *   &returns: Error.
 */
char *tcp_server_open(struct tcp_server_t **server, uint16_t port)
{
#define onexit if(sys_issock(sock)) sys_closesocket(sock);
	int val;
	struct sockaddr_in addr;
	sys_sock_t sock = sys_badsock;

	chkfail(sys_socket(&sock, AF_INET, SOCK_STREAM, 0));

	val = 1;
	chkfail(sys_setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(int)));

	memset(&addr, 0x00, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(port);

	chkfail(sys_bind(sock, (struct sockaddr *)&addr, sizeof(addr)));

	*server = malloc(sizeof(struct tcp_server_t));
	(*server)->sock = sock;

	return NULL;
#undef onexit
}

/**
 * Close a server.
 *   @server: The server.
 */
void tcp_server_close(struct tcp_server_t *server)
{
	sys_closesocket(server->sock);
	free(server);
}


/**
 * Retrieve the server file descriptor.
 *   @server: The server.
 *   &returns: The file descriptor.
 */
sys_sock_t tcp_server_sock(struct tcp_server_t *server)
{
	return server->sock;
}

/**
 * Retrieve the poll information from a server.
 *   @server: The server.
 *   &returns: The poll information.
 */
struct sys_poll_t tcp_server_poll(struct tcp_server_t *server)
{
	return sys_poll_sock(server->sock, sys_poll_in_e);
}


/**
 * Listen on a server.
 *   @server: The server.
 *   &returns: Error.
 */
char *tcp_server_listen(struct tcp_server_t *server)
{
#define onexit
	chkfail(sys_listen(server->sock, SOMAXCONN));
	
	return NULL;
#undef onexit
}

/**
 * Accept a new connection from the server.
 *   @server: The server.
 *   @fd: Ref. The output file descriptor.
 *   &returns: Error.
 */
char *tcp_server_accept(struct tcp_server_t *server, sys_sock_t *fd)
{
#define onexit
	socklen_t size;
	struct sockaddr_in addr;
	
	size = sizeof(addr);
	chkfail(sys_accept(server->sock, fd, (struct sockaddr *)&addr, &size));

	return NULL;
#undef onexit
}
