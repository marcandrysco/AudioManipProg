#include "../common.h"
#include <sys/stat.h>


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
 *   &returns: True if successful.
 */
bool fs_mkdir(const char *path, uint16_t perm)
{
	if(mkdir(path, perm) < 0)
		return false;

	if(chmod(path, perm) < 0)
		return false;

	return true;
}
