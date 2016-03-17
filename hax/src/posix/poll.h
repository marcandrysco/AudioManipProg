#ifndef POSIX_POLL_H
#define POSIX_POLL_H

/**
 * Poll enumerator.
 *   @poll_in_e: Input.
 *   @poll_out_e: Output.
 *   @poll_err_e: Error.
 */
enum sys_poll_e {
	sys_poll_in_e = 0x01,
	sys_poll_out_e = 0x02,
	sys_poll_err_e = 0x04,
};

/**
 * Poll structure.
 *   @fd: The file descriptor.
 *   @sock: The socket.
 *   @events, revents: The listen and received events.
 */
struct sys_poll_t {
	sys_fd_t fd;
	sys_sock_t sock;
	enum sys_poll_e events, revents;
};

/**
 * Create a poll structure for a file descriptor.
 *   @fd: The file descriptor.
 *   @events: The events.
 *   &returns: The poll structure.
 */
static inline struct sys_poll_t sys_poll_fd(sys_fd_t fd, enum sys_poll_e events)
{
	return (struct sys_poll_t){ fd, -1, events, 0 };
}

/**
 * Create a poll structure for a socket.
 *   @sock: The socket.
 *   @events: The events.
 *   &returns: The poll structure.
 */
static inline struct sys_poll_t sys_poll_sock(sys_sock_t sock, enum sys_poll_e events)
{
	return (struct sys_poll_t){ -1, sock, events, 0 };
}

/*
 * poll declarations
 */
bool sys_poll(struct sys_poll_t *poll, unsigned int n, int timeout);

#endif
