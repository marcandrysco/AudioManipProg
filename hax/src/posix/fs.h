#ifndef POSIX_FS_H
#define POSIX_FS_H

/*
 * file system declarations
 */
extern sys_fd_t sys_badfd;

bool fs_exists(const char *path);

char *fs_mkdir(const char *path, uint16_t perm);

#endif
