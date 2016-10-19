#ifndef TCP_H
#define TCP_H

/*
 * tcp client declarations
 */
struct tcp_client_t;

struct tcp_client_t *tcp_client_new(sys_fd_t fd);
void tcp_client_delete(struct tcp_client_t *client);

char *tcp_client_open(struct tcp_client_t **client, const char *host, uint16_t port);
void tcp_client_close(struct tcp_client_t *client);

sys_fd_t tcp_client_fd(struct tcp_client_t *client);
struct sys_poll_t tcp_client_poll(struct tcp_client_t *client);

size_t tcp_client_avail(struct tcp_client_t *client);
size_t tcp_client_queue(struct tcp_client_t *client);

bool tcp_client_read(struct tcp_client_t *client, void *restrict buf, size_t nbytes);
void tcp_client_write(struct tcp_client_t *client, const void *restrict buf, size_t nbytes);
bool tcp_client_proc(struct tcp_client_t *client, enum sys_poll_e events);

/*
 * tcp server declarations
 */
struct tcp_server_t;

char *tcp_server_open(struct tcp_server_t **server, uint16_t port);
void tcp_server_close(struct tcp_server_t *server);

char *tcp_server_listen(struct tcp_server_t *server);
char *tcp_server_accept(struct tcp_server_t *server, sys_fd_t *fd);

sys_fd_t tcp_server_fd(struct tcp_server_t *server);
struct sys_poll_t tcp_server_poll(struct tcp_server_t *server);

#endif
