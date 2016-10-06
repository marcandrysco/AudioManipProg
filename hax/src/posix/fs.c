#include "../common.h"
#include <sys/stat.h>


/*
 * global variables
 */
sys_fd_t sys_badfd = -1;


/**
 * Check if a file path exists.
 *   @path: The paht.
 *   &returns: The exists.
 */
bool fs_exists(const char *path)
{
	return access(path, F_OK) != -1;
}



/**
 * Create a directory.
 *   @path: the path.
 *   @perm: The permission.
 *   &returns: Error.
 */ 
char *fs_mkdir(const char *path, uint16_t perm)
{
	if(mkdir(path, perm) < 0)
		return mprintf("Failed to create directory '%s'. %s.", path, strerror(errno));

	if(chmod(path, perm) < 0)
		return mprintf("Failed to change permissions on directory '%s'. %s.", path, strerror(errno));

	return NULL;
}
