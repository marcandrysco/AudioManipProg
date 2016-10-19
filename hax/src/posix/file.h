#ifndef POSIX_FILE_H
#define POSIX_FILE_H

/*
 * file declarations
 */
extern sys_fd_t sys_badfd;

bool sys_isfd(sys_fd_t fd);
void sys_close(sys_fd_t fd);

#endif
