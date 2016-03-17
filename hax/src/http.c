#include "common.h"


/**
 * State enumerator.
 *   @head_v: Header.
 *   @body_v: Body.
 *   @done_v: Done.
 */
enum state_e {
	head_v,
	body_v,
	done_v
};

/**
 * HTTP server structure.
 *   @tcp: The TCP server.
 *   @cnt: The number of active clients.
 *   @client: The client list.
 */
struct http_server_t {
	struct tcp_server_t *tcp;

	unsigned int cnt;
	struct http_client_t *client;
};

/**
 * Connection structure.
 *   @tcp: The TCP client.
 *   @state: The state.
 *   @idx, len: The index and length.
 *   @buf: The buffer.
 *   @head: The request header.
 *   @prev, next: The previous and next clients.
 */
struct http_client_t {
	struct tcp_client_t *tcp;

	enum state_e state;
	unsigned int idx, len;
	struct strbuf_t buf;
	struct http_head_t head;

	struct http_client_t *prev, *next;
};


/*
 * local declarations
 */
static void client_resp(struct http_client_t *client, http_handler_f func, void *arg);

static bool isdelim(int16_t byte);
static bool iseol(int16_t byte);
static int16_t skiphspace(const char **str);


/**
 * Open an HTTP server.
 *   @server: Ref. The server.
 *   @port: The port.
 *   &returns: Error.
 */
char *http_server_open(struct http_server_t **server, uint16_t port)
{
#define onexit if((*server)->tcp != NULL) tcp_server_close((*server)->tcp); free(*server);
	*server = malloc(sizeof(struct http_server_t));
	(*server)->tcp = NULL;
	(*server)->client = NULL;
	(*server)->cnt = 0;
	chkfail(tcp_server_open(&(*server)->tcp, port));
	chkfail(tcp_server_listen((*server)->tcp));

	return NULL;
#undef onexit
}

/**
 * Close an HTTP server.
 *   @server: The server.
 */
void http_server_close(struct http_server_t *server)
{
	struct http_client_t *cur, *next;

	for(cur = server->client; cur != NULL; cur = next) {
		next = cur->next;
		http_client_delete(cur);
		printf("del?\n");
	}

	tcp_server_close(server->tcp);
	free(server);
}

/**
 * Create a new HTTP client.
 *   @tcp: The TCP client.
 *   &returns: The client.
 */
struct http_client_t *http_client_new(struct tcp_client_t *tcp)
{
	struct http_client_t *client;

	client = malloc(sizeof(struct http_client_t));
	client->state = head_v;
	client->tcp = tcp;
	client->buf = strbuf_init(256);

	return client;
}

/**
 * Delete a HTTP client.
 *   @client: The client.
 */
void http_client_delete(struct http_client_t *client)
{
	if(client->state != head_v)
		http_head_destroy(&client->head);

	strbuf_destroy(&client->buf);
	tcp_client_delete(client->tcp);
	free(client);
}


/**
 * Process data on a client.
 *   @client: The client.
 *   @func: The handler.
 *   @arg: The argument.
 *   &returns: Continue flag. False terminates connection.
 */
bool http_client_proc(struct http_client_t *client, http_handler_f func, void *arg)
{
	char ch;

	while(true) {
		if((client->state == done_v) && (tcp_client_queue(client->tcp) == 0))
			return false;

		if(!tcp_client_read(client->tcp, &ch, 1))
			break;

		if(client->state == head_v) {
			if(ch == '\r')
				continue;

			if((ch == '\n') && (strbuf_last(&client->buf) == '\n')) {
				const char *len;

				http_head_parse(&client->head, strbuf_finish(&client->buf));

				len = http_head_lookup(&client->head, "Content-Length");
				if(len != NULL)
					client->len = strtoul(len, (char **)&len, 0);
				else
					client->len = 0;

				if(client->len == 0) {
					client->state = done_v;
					client_resp(client, func, arg);
				}
				else
					client->state = body_v;

			}
			else if((ch == '\n') || ((ch >= 0x20) && (ch <= 0x7F)))
				strbuf_addch(&client->buf, ch);
		}
		else if(client->state == body_v) {
			strbuf_addch(&client->buf, ch);
			if(client->buf.idx == client->len) {
				client->state = done_v;
				client_resp(client, func, arg);
			}
		}
	}

	return true;
}

/**
 * Respond to a client.
 *   @client: The client.
 *   @func: The handler function.
 *   @arg: The argument.
 */
static void client_resp(struct http_client_t *client, http_handler_f func, void *arg)
{
	bool suc;
	char *buf;
	struct io_file_t file;
	struct http_args_t args;
	struct http_pair_t *pair;

	file = io_file_fd(tcp_client_sock(client->tcp));

	args.body = strbuf_finish(&client->buf);
	args.req = client->head;
	args.resp = http_head_init();
	args.file = io_file_accum(&buf);
	suc = func(args.req.path, &args, arg);
	io_file_close(args.file);

	if(suc) {
		char len[32];

		hprintf(file, "HTTP/1.1 200 OK\n");

		if(http_head_lookup(&args.resp, "Content-Type") == NULL)
			http_head_add(&args.resp, "Content-Type", "application/xhtml+xml");

		snprintf(len, sizeof(len), "%zu", strlen(buf));
		http_head_add(&args.resp, "Content-Length", len);
		http_head_add(&args.resp, "Connection", "close");

		for(pair = args.resp.pair; pair != NULL; pair = pair->next)
			hprintf(file, "%s: %s\n", pair->key, pair->value);

		hprintf(file, "\n");
		hprintf(file, buf);
	}
	else
		hprintf(file, "HTTP/1.1 404 Not Found\nContent-Length: 9\nConnection: close\n\nNot Found");

	free(buf);
	io_file_close(file);
	http_head_destroy(&args.resp);
}


/**
 * Process data on the server.
 *   @server: The server.
 *   @fds: The file descriptor information returned from poll.
 *   @func: The handler.
 *   @arg: The argument.
 *   &returns: Error.
 */
char *http_server_proc(struct http_server_t *server, struct sys_poll_t *fds, http_handler_f func, void *arg)
{
#define onexit
	bool cont[server->cnt];
	unsigned int i;
	struct http_client_t *client, **cur;

	for(i = 0, client = server->client; client != NULL; i++, client = client->next) {
		assert(i < server->cnt);

		cont[i] = true;
		if((fds != NULL) && (fds[i+1].revents == 0))
		if(fds[i+1].revents == 0)
			continue;

		tcp_client_proc(client->tcp, fds[i+1].revents);
		cont[i] = http_client_proc(client, func, arg);
	}

	for(i = 0, cur = &server->client; *cur != NULL; i++) {
		if(!cont[i]) {
			client = *cur;
			*cur = (*cur)->next;

			http_client_delete(client);
			server->cnt--;
		}
		else
			cur = &(*cur)->next;
	}

	if((fds == NULL) || (fds[0].revents != 0)) {
		sys_sock_t sock;
		struct http_client_t *client;

		chkfail(tcp_server_accept(server->tcp, &sock));

		client = http_client_new(tcp_client_new(sock));
		client->next = server->client;
		server->client = client;
		server->cnt++;

		http_client_proc(client, func, arg);
	}

	return NULL;
#undef onexit
}

/**
 * Retrieve the polling file descriptor set from the HTTP server.
 *   @server: The server.
 *   @poll: Optional. The pointer where to store the poll information.
 *   &returns: The number of file descriptors.
 */
unsigned int http_server_poll(struct http_server_t *server, struct sys_poll_t *poll)
{
	unsigned int i;
	struct http_client_t *client;

	if(poll != NULL) {
		poll[0] = tcp_server_poll(server->tcp);

		for(i = 1, client = server->client; client != NULL; i++, client = client->next)
			poll[i] = tcp_client_poll(client->tcp);
	}

	return 1 + server->cnt;
}

/**
 * Initialize a header structure.
 *   &returns: The header structure.
 */
struct http_head_t http_head_init(void)
{
	struct http_head_t head;

	head.verb = head.path = head.proto = NULL;
	head.pair = NULL;

	return head;
}

/**
 * Destroy a header sturcture.
 *   @head: The header.
 */
void http_head_destroy(struct http_head_t *head)
{
	struct http_pair_t *cur;

	for(cur = head->pair; cur != NULL; cur = head->pair) {
		head->pair = cur->next;
		free(cur->key);
		free(cur->value);
		free(cur);
	}

	erase(head->verb);
	erase(head->path);
	erase(head->proto);
}



/**
 * Lookup the value from the header.
 *   @head: The ehader.
 *   @key: The key.
 *   &returns: The value or null.
 */
const char *http_head_lookup(struct http_head_t *head, const char *key)
{
	struct http_pair_t *pair;

	for(pair = head->pair; pair != NULL; pair = pair->next) {
		if(strcmp(key, pair->key) == 0)
			return pair->value;
	}

	return NULL;
}

/**
 * Add a key-value pair to the header.
 *   @head: The header.
 *   @key: The key.
 *   @value: The value.
 */
void http_head_add(struct http_head_t *head, const char *key, const char *value)
{
	struct http_pair_t **pair;

	for(pair = &head->pair; *pair != NULL; pair = &(*pair)->next) {
		if(strcmp(key, (*pair)->key) == 0)
			break;
	}

	if(*pair == NULL) {
		*pair = malloc(sizeof(struct http_pair_t));
		(*pair)->key = strdup(key);
		(*pair)->value = strdup(value);
		(*pair)->next = NULL;
	}
	else
		strset(&(*pair)->value, mprintf("%s,%s", (*pair)->value, value));
}


/**
 * Parse a header.
 *   @head: Ref. The header.
 *   @str: The string.
 *   &returns: The error.
 */
char *http_head_parse(struct http_head_t *head, const char *str)
{
#define onexit strbuf_destroy(&buf); http_head_destroy(head); *head = http_head_init();
	struct strbuf_t buf;

	*head = http_head_init();
	buf = strbuf_init(32);

	if(iseol(skiphspace(&str)))
		fail("Invalid header. Missing verb.");

	while(!isdelim(*str))
		strbuf_addch(&buf, *str++);

	head->verb = strbuf_done(&buf);
	buf = strbuf_init(32);

	if(iseol(skiphspace(&str)))
		fail("Invalid header. Missing path.");

	while(!isdelim(*str))
		strbuf_addch(&buf, *str++);

	head->path = strbuf_done(&buf);
	buf = strbuf_init(32);

	if(iseol(skiphspace(&str)))
		fail("Invalid header. Missing protocol.");

	while(!isdelim(*str))
		strbuf_addch(&buf, *str++);

	head->proto = strbuf_done(&buf);

	if(!iseol(skiphspace(&str)))
		fail("Invalid header. Invalid request.");

	while(true) {
		char *key, *value;

		while((*str == '\n') || (*str == '\r'))
			str++;

		if(*str <= 0)
			break;

		buf = strbuf_init(32);
		while(*str != ':') {
			if(iseol(*str))
				fail("Invalid header. Missing value.");

			strbuf_addch(&buf, *str++);
		}

		key = strbuf_done(&buf);
		str++;
		skiphspace(&str);

		buf = strbuf_init(32);
		while(!iseol(*str))
			strbuf_addch(&buf, *str++);

		value = strbuf_done(&buf);
		http_head_add(head, key, value);

		free(key);
		free(value);
	}

	return NULL;
#undef onexit
}


/**
 * Check if a byte is a delimiter (space or end-of-stream).
 *   @byte: The byte.
 *   &returns: True if delimiter.
 */
static bool isdelim(int16_t byte)
{
	return isspace(byte) || (byte <= 0);
}

/**
 * Check if a byte is end end-of-line or end-of-stream.
 *   @byte: The byte.
 *   &returns: True if end-of-line.
 */
static bool iseol(int16_t byte)
{
	return (byte == '\n') || (byte == '\r') || (byte < 0);
}

/**
 * Skip spaces from an input.
 *   @input: The input.
 *   @byte: The current byte.
 *   &returns: The next byte after spaces.
 */
static int16_t skiphspace(const char **str)
{
	while((**str == ' ') || (**str == '\t'))
		(*str)++;

	return **str;
}
