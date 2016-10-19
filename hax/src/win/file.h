#ifndef WIN_FILE_H
#define WIN_FILE_H

/*
 * file declarations
 */
extern sys_fd_t sys_badfd;

bool sys_isfd(sys_fd_t fd);


/**
 * Create a file descriptor from a handle.
 *   @handle: The handle.
 *   &returns: The file descriptor.
 */
static inline sys_fd_t w32_fd(HANDLE handle)
{
	return (struct sys_fd_t){ handle, INVALID_SOCKET };
}

#endif
