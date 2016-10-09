#include "../common.h"

/*
 * global variables
 */
sys_fd_t sys_badfd = -1;


/**
 * Close a file descriptor.
 *   @fd: The file descriptor.
 */
void sys_close(sys_fd_t fd)
{
	close(fd);
}
