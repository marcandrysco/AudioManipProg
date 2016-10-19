#ifndef WIN_DEFS_H
#define WIN_DEFS_H

/**
 * File descriptor structure.
 *   @handle: Pollable handle.
 *   @sock: Optional socket.
 */
struct sys_fd_t {
	HANDLE handle;
	SOCKET sock;
};

/*
 * common definitions 
 */
typedef struct sys_fd_t sys_fd_t;
typedef int socklen_t;

#endif
