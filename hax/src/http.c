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

/**
 * Asynchronous information structure.
 *   @server: Server.
 *   @func: The function.
 *   @arg: The argument.
 */
struct async_t {
	struct http_server_t *server;

	http_handler_f func;
	void *arg;
};


/*
 * local declarations
 */
static void server_async(sys_fd_t fd, void *arg);

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
	}

	tcp_server_close(server->tcp);
	free(server);
}


/**
 * Create an asynchronous server.
 *   @port: The port.
 *   @func: The function.
 *   @arg: The argument.
 *   &returns: The task.
 */
struct sys_task_t *http_server_async(uint16_t port, http_handler_f func, void *arg)
{
	struct async_t *async;

	async = malloc(sizeof(struct async_t));
	async->func = func;
	async->arg = arg;
	http_server_open(&async->server, port);

	return sys_task_new(server_async, async);
}
/**
 * Asynchronous task callback.
 *   @fd: The synchronization file.
 *   @arg: The arugment.
 */
static void server_async(sys_fd_t fd, void *arg)
{
	struct async_t *async = arg;

	while(true) {
		unsigned int nfds = 1 + http_server_poll(async->server, NULL);
		struct sys_poll_t poll[nfds];

		poll[0] = sys_poll_fd(fd, sys_poll_in_e);
		http_server_poll(async->server, poll + 1);

		sys_poll(poll, nfds, -1);
		if(poll[0].revents)
			break;

		http_server_proc(async->server, poll + 1, async->func, async->arg);
	}

	http_server_close(async->server);
	free(async);
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
	tcp_client_close(client->tcp);
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
	bool cont = false;

	while(true) {
		if((client->state == done_v) && (tcp_client_queue(client->tcp) == 0))
			return false;

		if(!tcp_client_read(client->tcp, &ch, 1))
			break;

		cont = true;
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
					client->state = head_v;
					client_resp(client, func, arg);
					strbuf_reset(&client->buf);
					http_head_destroy(&client->head);
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
				client->state = head_v;
				client_resp(client, func, arg);
				strbuf_reset(&client->buf);
				http_head_destroy(&client->head);
			}
		}
	}

	return cont;
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
	void *buf;
	size_t nbytes;
	struct io_file_t file;
	struct http_args_t args;
	struct http_pair_t *pair;

	file = io_file_fd(tcp_client_fd(client->tcp));

	args.code = 200;
	args.body = strbuf_finish(&client->buf);
	args.req = client->head;
	args.resp = http_head_init();
	args.file = io_file_buf(&buf, &nbytes);
	suc = func(args.req.path, &args, arg);
	io_file_close(args.file);

	if(suc) {
		char len[32];

		hprintf(file, "HTTP/1.1 %u %s\n", args.code, args.code == 200 ? "OK" : "Redirect");

		if(http_head_lookup(&args.resp, "Content-Type") == NULL)
			http_head_add(&args.resp, "Content-Type", "application/xhtml+xml");

		snprintf(len, sizeof(len), "%u", (unsigned int)nbytes);
		http_head_add(&args.resp, "Content-Length", len);
		http_head_add(&args.resp, "Connection", "keep-alive");

		for(pair = args.resp.pair; pair != NULL; pair = pair->next)
			hprintf(file, "%s: %s\n", pair->key, pair->value);

		hprintf(file, "\n");
		io_file_write(file, buf, nbytes);
	}
	else
		hprintf(file, "HTTP/1.1 404 Not Found\nContent-Length: 9\nConnection: keep-alive\n\nNot Found");

	free(buf);
	io_file_close(file);
	http_head_destroy(&args.resp);
}

struct http_resp_t {
	void *buf;
	size_t nbytes;
	struct io_file_t file;
	struct http_args_t args;
};

void client_send(struct http_resp_t *resp)
{
	struct http_pair_t *pair;
	char len[32];

	io_file_close(resp->args.file);
	hprintf(resp->file, "HTTP/1.1 %u %s\n", resp->args.code, resp->args.code == 200 ? "OK" : "Redirect");

	if(http_head_lookup(&resp->args.resp, "Content-Type") == NULL)
		http_head_add(&resp->args.resp, "Content-Type", "application/xhtml+xml");

	snprintf(len, sizeof(len), "%u", (unsigned int)resp->nbytes);
	http_head_add(&resp->args.resp, "Content-Length", len);
	http_head_add(&resp->args.resp, "Connection", "close");

	for(pair = resp->args.resp.pair; pair != NULL; pair = pair->next)
		hprintf(resp->file, "%s: %s\n", pair->key, pair->value);

	hprintf(resp->file, "\n");
	io_file_write(resp->file, resp->buf, resp->nbytes);

	free(resp->buf);
	io_file_close(resp->file);
	http_head_destroy(&resp->args.resp);
	free(resp);
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
		if(fds[i+1].revents == 0)
			continue;
		else if(fds[i+1].revents == sys_poll_err_e)
			cont[i] = false;
		else {
			tcp_client_proc(client->tcp, fds[i+1].revents);
			cont[i] = http_client_proc(client, func, arg);
		}
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
		sys_fd_t fd;
		struct http_client_t *client;

		chkabort(tcp_server_accept(server->tcp, &fd));

		client = http_client_new(tcp_client_new(fd));
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


/**
 * Creat a key-value pair.
 *   @key: Consumed. The key.
 *   @value: Consumed. The value.
 *   &returns: The pair.
 */
struct http_pair_t *http_pair_new(char *key, char *value)
{
	struct http_pair_t *pair;

	pair = malloc(sizeof(struct http_pair_t));
	*pair = (struct http_pair_t){ key, value, NULL };

	return pair;
}

/**
 * Clear an entire list of pairs.
 *   @pair: The pair list.
 */
void http_pair_clear(struct http_pair_t *pair)
{
	struct http_pair_t *cur;

	while(pair != NULL) {
		cur = pair;
		pair = cur->next;

		free(cur->key);
		free(cur->value);
		free(cur);
	}
}


/**
 * Obtain the length of the pair list.
 *   @pair: The pair list.
 *   &returns: The length.
 */
unsigned int http_pair_len(struct http_pair_t *pair)
{
	unsigned int n;

	for(n = 0; pair != NULL; n++)
		pair = pair->next;

	return n;
}

/**
 * Retrieve the tail pair.
 *   @pair: The pair reference.
 *   &returns: The tail reference.
 */
struct http_pair_t **http_pair_tail(struct http_pair_t **pair)
{
	while(*pair != NULL)
		pair = &(*pair)->next;

	return pair;
}

/**
 * Append a key-value pair onto the pair list.
 *   @pair: The pair list reference.
 *   @key: Consumed. The key.
 *   @value: Consumed. The value.
 */
void http_pair_append(struct http_pair_t **pair, char *key, char *value)
{
	*http_pair_tail(pair) = http_pair_new(key, value);
}

/**
 * Find a pair reference by key.
 *   @pair: The pair reference.
 *   @key: The key.
 *   &returns: The found pair reference or null.
 */
struct http_pair_t **http_pair_find(struct http_pair_t **pair, const char *key)
{
	while(*pair != NULL) {
		if(strcmp((*pair)->key, key) == 0)
			return pair;

		pair = &(*pair)->next;
	}

	return NULL;
}

/**
 * Retrieve a value from a pair list.
 *   @pair: The pair list.
 *   @key: The key.
 *   &returns: The value or null.
 */
const char *http_pair_get(struct http_pair_t *pair, const char *key)
{
	struct http_pair_t **find;

	find = http_pair_find(&pair, key);

	return find ? (*find)->value : NULL;
}


/**
 * Retrieve values from the pair list using a format string.
 *   @pair: The pair list.
 *   @fmt: The format string.
 *   @...: Format arguments.
 *   &returns: Error.
 */
char *http_pair_getf(struct http_pair_t *pair, const char *restrict fmt, ...)
{
	char *find;
	va_list args;
	unsigned int n = 0;

	va_start(args, fmt);

	while(true) {
		while(isspace(*fmt))
			fmt++;

		if(*fmt == '\0')
			break;
		else if(*fmt == '$') {
			if(fmt[1] != '\0')
				fatal("Invalid format string. Text after '$'.");
			else if(http_pair_len(pair) != n)
				return mprintf("Extra pairs in list.");

			break;
		}

		find = strchr(fmt, ':');
		if(find == NULL)
			fatal("Invalid format string.");

		{
			unsigned int len = find - fmt;
			char id[len+1];
			const char *value;

			memcpy(id, fmt, len);
			id[len] = '\0';

			value = http_pair_get(pair, id);
			if(value == NULL)
				return mprintf("Cannot find key '%s'.", id);

			fmt = find + 1;
			switch(*fmt) {
			case 's':
				*va_arg(args, const char **) = value;
				break;

			default:
				fatal("Invalid format string type '%c'.", *fmt);
			}

			n++, fmt++;
		}
	}

	va_end(args);

	return NULL;
}


/**
 * Create a cookie string from a list of pairs.
 *   @pair: The list of pairs.
 *   &returns: The allocated coookie string.
 */
char *http_cookies_string(struct http_pair_t *pair)
{
	struct strbuf_t buf;

	buf = strbuf_init(64);

	while(pair != NULL) {
		strbuf_addstr(&buf, pair->key);
		strbuf_addch(&buf, '=');
		strbuf_addstr(&buf, pair->value);

		if(pair->next == NULL)
			break;

		pair = pair->next;
		strbuf_addch(&buf, ';');
	}

	return strbuf_done(&buf);
}

/**
 * Parse cookies.
 *   @str: The string.
 *   &returns: The cookies list.
 */
struct http_pair_t *http_cookies_parse(const char *str)
{
	const char *key, *val;
	unsigned int keylen;
	struct http_pair_t *head = NULL, **cur = &head;

	while(true) {
		key = str;
		while((*str != '=') && (*str != '\0'))
			str++;

		if(*str == '\0')
			return head;

		keylen = str - key;
		val = ++str;
		while((*str != ';') && (*str != '\0'))
			str++;

		*cur = http_pair_new(strndup(key, keylen), strndup(val, str - val));
		http_cookie_sanitize((*cur)->key);
		http_cookie_sanitize((*cur)->value);
		cur = &(*cur)->next;

		if(*str == '\0')
			break;

		str++;
	}

	return head;
}

/**
 * Sanitize a key or value string for a cookie. All invalid characters are
 * replaced with '~'.
 *   @str: The string.
 */
void http_cookie_sanitize(char *str)
{
	unsigned int i;

	for(i = 0; str[i] != '\0'; i++) {
		if(!isalnum(str[i]) && (strchr("!#$%&'()*+-./:<=>?@[]^_`{|}~", str[i]) == NULL))
			str[i] = '~';
	}
}


/**
 * Parse form data.
 *   @pair: Ref. The reference to the returned pair list.
 *   @str: The input string.
 *   &returns: The error.
 */
char *http_form_parse(struct http_pair_t **pair, const char *str)
{
#define onexit strbuf_destroy(&key); strbuf_destroy(&value); http_pair_clear(*orig); *orig = NULL;
	struct strbuf_t key, value;
	struct http_pair_t **orig = pair;

	while(true) {
		key = strbuf_init(16);
		value = strbuf_init(16);

		while((*str != '=') && (*str != '\0')) {
			if(isalnum(*str))
				strbuf_addch(&key, *str);
			else if(*str == '+')
				strbuf_addch(&key, ' ');
			else if(*str == '%') {
				int hi, lo;

				str++;
				if((hi = ch_hex2int(*str++)) < 0)
					fail("Invalid form data.");
				else if((lo = ch_hex2int(*str++)) < 0)
					fail("Invalid form data.");

				strbuf_addch(&key, (char)(hi * 0x10 + lo));
			}

			str++;
		}

		if(*str == '\0')
			fail("Invalid form data.");

		str++;

		while((*str != '&') && (*str != '\0')) {
			if(isalnum(*str))
				strbuf_addch(&value, *str), str++;
			else if(*str == '+')
				strbuf_addch(&value, ' '), str++;
			else if(*str == '%') {
				int hi, lo;

				str++;
				if((hi = ch_hex2int(*str++)) < 0)
					fail("Invalid form data.");
				else if((lo = ch_hex2int(*str++)) < 0)
					fail("Invalid form data.");

				strbuf_addch(&value, (char)(hi * 0x10 + lo));
			}
		}

		*pair = http_pair_new(strbuf_done(&key), strbuf_done(&value));
		pair = &(*pair)->next;

		if(*str == '\0')
			break;

		str++;
	}

	*pair = NULL;

	return NULL;
#undef onexit
}


/**
 * Process an asset list.
 *   @assets: The asset list.
 *   @path: The path.
 *   @args: The argument.
 *   @prefix: The prefix.
 *   &returns: True if handled, false otherwise.
 */
bool http_asset_proc(struct http_asset_t *assets, const char *path, struct http_args_t *args, const char *prefix)
{
	while(assets->req != NULL) {
		if(strcmp(path, assets->req) == 0)
			break;

		assets++;
	}

	if(assets->req == NULL)
		return false;

	if(prefix == NULL)
		prefix = "";

	{
		unsigned int len = lprintf("%s%s", prefix, assets->path);
		char full[len+1];
		FILE *file;
		size_t rd;
		uint8_t buf[4096];

		sprintf(full, "%s%s", prefix, assets->path);

		file = fopen(full, "r");
		if(file == NULL)
			return false;

		while(true) {
			rd = fread(buf, 1, 4096, file);
			if(rd == 0)
				break;

			io_file_write(args->file, buf, rd);
		}

		fclose(file);
	}

	http_head_add(&args->resp, "Content-Type", assets->type);

	return true;
}
