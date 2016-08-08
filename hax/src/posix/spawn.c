#include "../common.h"
#include <spawn.h>
#include <sys/wait.h>


/**
 * Spawn a thread.
 *   @pid: Ref. The process ID.
 *   @path: The image path.
 *   @argv: The argument array.
 *   &returns: Error.
 */
char *sys_spawn(sys_pid_t *pid, const char *path, char *const *argv)
{
	if(posix_spawn(pid, path, NULL, NULL, argv, NULL) < 0)
		return mprintf("Failed to spawn process executing '%s'. %s.", path, strerror(errno));

	return NULL;
}

/**
 * Wait on a process.
 *   @pid: The process ID.
 *   &returns: The status code.
 */
int sys_wait(sys_pid_t pid)
{
	int status;

	waitpid(pid, &status, 0);

	return 0;
}
