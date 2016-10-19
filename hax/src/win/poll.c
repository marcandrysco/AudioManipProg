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
	unsigned int i;
	DWORD ret;
	HANDLE handle[n];

	for(i = 0; i < n; i++)
		handle[i] = list[i].fd.handle;

	ret = WaitForMultipleObjects(n, handle, FALSE, (timeout >= 0) ? timeout : INFINITE);

	if(ret >= 0x80)
		return false;

	list[ret].revents = (sys_poll_in_e | sys_poll_out_e) & list[ret].events;

	return true;
}
