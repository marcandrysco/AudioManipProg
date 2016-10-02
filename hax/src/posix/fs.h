#ifndef POSIX_FS_H
#define POSIX_FS_H

/*
 * file system declarations
 */
bool fs_exists(const char *path);

bool fs_mkdir(const char *path, uint16_t perm);

#endif
