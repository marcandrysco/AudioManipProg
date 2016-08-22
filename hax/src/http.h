#ifndef HTTP_H
#define HTTP_H

/**
 * Header sturcture.
 *   @verb, path, proto: Verb path and protocol.
 *   @pair: The key-value pairs.
 */
struct http_head_t {
	char *verb, *path, *proto;

	struct http_pair_t *pair;
};

/**
 * Key-value pair structure.
 *   @key, value: Key and value strings.
 *   @next: The next pair.
 */
struct http_pair_t {
	char *key, *value;

	struct http_pair_t *next;
};

/**
 * Argument structure.
 *   @file: The output file.
 *   @body: The body.
 *   @req, resp: The request and response header.
 */
struct http_args_t {
	struct io_file_t file;

	const char *body;
	struct http_head_t req, resp;
};


/**
 * Server handler callback.
 *   @path: The path.
 *   @args: The request arguments.
 *   @arg: The argument.
 *   &returns: True if handled, false otherwise.
 */
typedef bool (*http_handler_f)(const char *path, struct http_args_t *args, void *arg);

/*
 * structure prototypes
 */
struct tcp_client_t;
struct http_server_t;
struct http_client_t;

/*
 * http server function declarations
 */
char *http_server_open(struct http_server_t **server, uint16_t port);
void http_server_close(struct http_server_t *server);

struct sys_task_t *http_server_async(uint16_t port, http_handler_f func, void *arg);

char *http_server_proc(struct http_server_t *server, struct sys_poll_t *fds, http_handler_f func, void *arg);
unsigned int http_server_poll(struct http_server_t *server, struct sys_poll_t *poll);

/*
 * http client function declarations
 */
struct http_client_t *http_client_new(struct tcp_client_t *tcp);
void http_client_delete(struct http_client_t *client);

bool http_client_proc(struct http_client_t *client, http_handler_f func, void *arg);

/*
 * http header function declarations
 */
struct http_head_t http_head_init(void);
void http_head_destroy(struct http_head_t *head);

const char *http_head_lookup(struct http_head_t *head, const char *key);
void http_head_add(struct http_head_t *head, const char *key, const char *value);

char *http_head_parse(struct http_head_t *head, const char *str);

#endif
