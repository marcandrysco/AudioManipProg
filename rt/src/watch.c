#include "common.h"
#include <poll.h>
#include <pthread.h>
#include <sys/inotify.h>


/**
 * Fatally quit the program. This function does not return.
 *   @format: The error message format.
 *   @...: The printf-style arguments.
 */

void fatal(const char *restrict format, ...)
{
	va_list args;

	va_start(args, format);
	vfprintf(stderr, format, args);
	va_end(args);

	fputc('\n', stderr);
	abort();
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
 * Notifier structure.
 *   @list: The file list.
 *   @pipe: The synchronization pipe.
 *   @thread: The thread.
 *   @notify: The notify callback.
 *   @arg: The notify argument.
 */

struct amp_notify_t {
	char **list;

	int pipe[2];
	pthread_t thread;

	amp_notify_f func;
	void *arg;
};


/*
 * local declarations
 */

static void *notify_thread(void *arg);


/**
 * Create a notifier.
 *   @list: The file list.
 *   @func: The callback function.
 *   @arg: The argument.
 *   &returns: The notifier.
 */

struct amp_notify_t *amp_notify_new(char **list, amp_notify_f func, void *arg)
{
	int err;
	struct amp_notify_t *notify;

	notify = malloc(sizeof(struct amp_notify_t));
	notify->list = list;
	notify->func = func;
	notify->arg = arg;

	if(pipe(notify->pipe) < 0)
		fatal("Cannot create pipe. %s.", strerror(errno));

	err = pthread_create(&notify->thread, NULL, notify_thread, notify);
	if(err != 0)
		fatal("Cannot create thread. %s.", strerror(err));

	return notify;
}

/**
 * Delete a notifier.
 *   @notify: The notifier.
 */

void amp_notify_delete(struct amp_notify_t *notify)
{
	int err;
	uint8_t cmd;

	cmd = 1;
	err = write(notify->pipe[1], &cmd, 1);
	if(err < 0)
		fprintf(stderr, "Failed to write to descriptor. %s.\n", strerror(errno)), exit(1);

	pthread_join(notify->thread, NULL);

	close(notify->pipe[0]);
	close(notify->pipe[1]);
	free(notify);
}

/**
 * Notify processing thread.
 *   @arg: The argument.
 *   &returns: Always null.
 */

static void *notify_thread(void *arg)
{
	int fd, ret;
	char **file;
	struct pollfd fdset[2];
	struct amp_notify_t *notify = arg;

	fd = inotify_init();

	for(file = notify->list; *file != NULL; file++) {
		const char *slash = strrchr(*file, '/');

		if(slash != NULL) {
			size_t len = slash-*file;
			char dir[len+1];

			memcpy(dir, *file, len);
			dir[len] = '\0';

			inotify_add_watch(fd, dir, IN_MODIFY | IN_MOVED_TO);
		}
		else
			inotify_add_watch(fd, ".", IN_MODIFY | IN_MOVED_TO);
	}

	fdset[0].fd = notify->pipe[0];
	fdset[1].fd = fd;

	while(true) {
		fdset[0].events = fdset[1].events = POLLIN;

		ret = poll(fdset, 2, -1);
		if(ret < 0)
			fatal("Poll failed. %s.", strerror(errno));

		if(fdset[0].revents & POLLIN)
			break;

		if(fdset[1].revents & POLLIN) {
			struct inotify_event *event;
			uint8_t buf[sizeof(struct inotify_event) + NAME_MAX + 1];

			if(read(fd, &buf, sizeof(buf)) < 0)
				fatal("Read failed. %s.", strerror(errno));

			event = (void *)buf;
			for(file = notify->list; *file != NULL; file++) {
				if(strcmp(hax_basename(*file), event->name) == 0)
					notify->func(*file, notify->arg);
			}
		}
	}

	close(fd);

	return NULL;
}
