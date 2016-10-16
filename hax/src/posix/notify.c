#include <sys/inotify.h>
#include "../common.h"


/**
 * Notify structure.
 *   @fd: The file descriptor.
 */
struct sys_notify_t {
	sys_fd_t fd;
	char name[NAME_MAX+1];
};


/**
 * Create a change notifier.
 *   &returns: The notifier.
 */
struct sys_notify_t *sys_notify_new(void)
{
	struct sys_notify_t *notify;

	notify = malloc(sizeof(struct sys_notify_t));
	notify->fd = inotify_init();

	return notify;
}

/**
 * Delete a change notifier.
 *   @notify: The notifier.
 */
void sys_notify_delete(struct sys_notify_t *notify)
{
	close(notify->fd);
	free(notify);
}


/**
 * Add a path to the change notifier.
 *   @notify: The notifier.
 *   @path: The path.
 *   @id: The identifier.
 *   &returns: Error.
 */
char *sys_notify_add(struct sys_notify_t *notify, const char *path, int *id)
{
	int ifd;

	ifd = inotify_add_watch(notify->fd, path, IN_MODIFY | IN_MOVED_TO | IN_ATTRIB);
	if(id != NULL)
		*id = ifd;

	return NULL;
}


/**
 * Retrieve the descriptor for the change notifier.
 *   @notify: The notifier.
 *   &returns: The descriptor.
 */
sys_fd_t sys_notify_fd(struct sys_notify_t *notify)
{
	return notify->fd;
}

/**
 * Retrieve the basename from the path. The returned pointer points into the
 * string passed in as 'path'; the returned value is valid so long as 'path'
 * is not modified.
 *   @path: The path.
 *   &returns; The basename.
 */
const char *hax_basename(const char *path)
{
	const char *ptr = strrchr(path, '/');

	return ptr ? (ptr + 1) : path;
}

/**
 * Process a poll of the notifier.
 *   @notify: The notifier.
 *   @poll: The poll array.
 */
void sys_notify_proc(struct sys_notify_t *notify, struct sys_poll_t *poll)
{
	if(poll->revents) {
		struct inotify_event *event;
		uint8_t buf[sizeof(struct inotify_event) + NAME_MAX + 1];

		if(read(notify->fd, &buf, sizeof(buf)) < 0)
			fatal("Read failed. %s.", strerror(errno));

		event = (void *)buf;
		strcpy(notify->name, event->name);
		//if(strcmp(hax_basename(notify->path), event->name) == 0)
			//notify->func(notify->path, notify->arg);
	}
}
