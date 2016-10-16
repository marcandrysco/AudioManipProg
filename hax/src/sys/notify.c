#include "../common.h"


/**
 * Notify task information.
 *   @notify: The change notifier.
 *   @func: The function.
 *   @arg: The argument.
 */
struct notify_async_t {
	struct sys_notify_t *notify;
	sys_change_f func;
	void *arg;
};

/*
 * local declarations
 */
static void notify_task(sys_fd_t fd, void *arg);


/**
 * Asynchronously poll a change notifier.
 *   @notify: the notifier.
 *   @func: the change function.
 *   @arg: the argument.
 *   &returns: the task.
 */
struct sys_task_t *sys_notify_async(struct sys_notify_t *notify, sys_change_f func, void *arg)
{
	struct notify_async_t *info;

	info = malloc(sizeof(struct notify_async_t));
	*info = (struct notify_async_t){ notify, func, arg };

	return sys_task_new(notify_task, info);
}

/**
 * Asynchronously poll a change notifier on a single path.
 *   @notify: the notifier.
 *   @func: the change function.
 *   @arg: the argument.
 *   &returns: the task.
 */
struct sys_task_t *sys_notify_async1(const char *path, sys_change_f func, void *arg)
{
	struct sys_notify_t *notify;

	notify = sys_notify_new();
	chkwarn(sys_notify_add(notify, path, NULL));

	return sys_notify_async(notify, func, arg);
}

/**
 * notify task thread.
 *   @fd: the synchronization file descriptor.
 *   @arg: the argument.
 */
static void notify_task(sys_fd_t fd, void *arg)
{
	struct notify_async_t info;

	info = *(struct notify_async_t *)arg;
	free(arg);

	while(true) {
		struct sys_poll_t fds[2];

		fds[0] = sys_poll_fd(fd, sys_poll_in_e);
		fds[1] = sys_poll_fd(sys_notify_fd(info.notify), sys_poll_in_e);
		sys_poll(fds, 2, -1);

		if(fds[0].revents)
			break;
		else if(fds[1].revents) {
			sys_notify_proc(info.notify, fds + 1);
			info.func(NULL, info.arg);
		}
	}

	sys_notify_delete(info.notify);
}
