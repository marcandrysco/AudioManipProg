#include "../common.h"


/**
 * Poll a single descriptor.
 *   @fd: The file descriptor.
 *   @events: The events to listen.
 *   @timeout: The timeout in milliseconds.
 *   &returns: The receieved event or zero on timeout.
 */
enum sys_poll_e sys_poll1(sys_fd_t fd, enum sys_poll_e events, int timeout)
{
	struct sys_poll_t info;

	info = sys_poll_fd(fd, events);
	sys_poll(&info, 1, timeout);

	return info.revents;
}
