#include "../common.h"


/**
 * Asynchronously poll files.
 *   @list: The poll structure list.
 *   @n: The number of poll structures.
 *   @timeout: The timeout in milliseconds. Negative waits forever.
 *   &returns: True if on file wakeup, false on timeout.
 */
bool sys_poll(struct sys_poll_t *list, unsigned int n, int timeout)
{
	int err;
	unsigned int i;
	struct pollfd fds[n];

	for(i = 0; i < n; i++) {
		fds[i].fd = (list[i].fd >= 0) ? list[i].fd : list[i].sock;
		fds[i].events = fds[i].revents = 0;
		fds[i].events |= (list[i].events & sys_poll_in_e) ? POLLIN : 0;
		fds[i].events |= (list[i].events & sys_poll_out_e) ? POLLOUT : 0;
		fds[i].events |= (list[i].events & sys_poll_err_e) ? POLLERR : 0;
	}

	do
		err = poll(fds, n, timeout);
	while((err < 0) && (errno == EINTR));

	if(err < 0)
		fatal("Poll failed. %s.", strerror(errno));

	for(i = 0; i < n; i++) {
		list[i].revents = 0;
		list[i].revents |= (fds[i].revents & POLLIN) ? sys_poll_in_e : 0;
		list[i].revents |= (fds[i].revents & POLLOUT) ? sys_poll_out_e : 0;
		list[i].revents |= (fds[i].revents & POLLERR) ? sys_poll_err_e : 0;
	}

	return err > 0;
}
