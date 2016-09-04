#include "../common.h"


/**
 * Check if a file path exists.
 *   @path: The paht.
 *   &returns: The exists.
 */
bool fs_exists(const char *path)
{
	return access(path, F_OK) != -1;
}
