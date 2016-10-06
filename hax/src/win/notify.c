#include "../common.h"


/**
 * Notify structure.
 *   @id: The current id.
 *   @inst: The instance list.
 */
struct sys_notify_t {
	int id;
	HANDLE event;
	struct sys_notify_inst_t *inst;
};

/**
 * Notify instance structure.
 *   @id: The identifier.
 *   @handle: The handle.
 *   @async: The overlapped asynchronous structure.
 *   @data: The data location.
 *   @next: The next instance.
 */
struct sys_notify_inst_t {
	int id;
	HANDLE handle;
	OVERLAPPED async;
	union {
		FILE_NOTIFY_INFORMATION info;
		uint8_t buf[sizeof(FILE_NOTIFY_INFORMATION) + 2 * MAX_PATH];
	} data;

	struct sys_notify_inst_t *next;
};


/**
 * Create a change notifier.
 *   &returns: The notifier.
 */
struct sys_notify_t *sys_notify_new(void)
{
	struct sys_notify_t *notify;

	notify = malloc(sizeof(struct sys_notify_t));
	notify->id = 1;
	notify->inst = NULL;
	notify->event = CreateEvent(NULL, TRUE, FALSE, NULL);

	return notify;
}

/**
 * Delete a change notifier.
 *   @notify: The notifier.
 */
void sys_notify_delete(struct sys_notify_t *notify)
{
	struct sys_notify_inst_t *inst;

	while(notify->inst != NULL) {
		inst = notify->inst;
		notify->inst = inst->next;

		CloseHandle(inst->handle);
		free(inst);
	}

	CloseHandle(notify->event);
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
	HANDLE handle;

	handle = CreateFile(path, FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL);
	if(handle == NULL)
		fatal("ERR: %C\n", w32_errstr());

	struct sys_notify_inst_t *inst;

	inst = malloc(sizeof(struct sys_notify_inst_t));
	inst->handle = handle;
	inst->next = notify->inst;
	notify->inst = inst;

	memset(&inst->async, 0x00, sizeof(OVERLAPPED));
	inst->async.hEvent = notify->event;

	ReadDirectoryChangesW(handle, &inst->data, sizeof(inst->data), TRUE, FILE_NOTIFY_CHANGE_LAST_WRITE, NULL, &inst->async, 0);

	/*
	HANDLE handle;

	handle = FindFirstChangeNotification(path, FALSE, FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE);
	if(handle == NULL)
		return mprintf("Failed to watch '%s'. %C.", w32_errstr());

	inst = malloc(sizeof(struct sys_notify_inst_t));
	inst->id = notify->id++;
	inst->handle = handle;
	inst->next = notify->inst;
	notify->inst = inst;

	if(id != NULL)
		*id = inst->id;

		*/
	return NULL;
}


/**
 * Retrieve the descriptor for the change notifier.
 *   @notify: The notifier.
 *   &returns: The descriptor.
 */
sys_fd_t sys_notify_fd(struct sys_notify_t *notify)
{
	return notify->event;
}

/**
 * Process a poll of the notifier.
 *   @notify: The notifier.
 *   @poll: The poll array.
 */
void sys_notify_proc(struct sys_notify_t *notify, struct sys_poll_t *poll)
{
	struct sys_notify_inst_t *inst;

	for(inst = notify->inst; inst != NULL; inst = inst->next) {
		DWORD len;

		GetOverlappedResult(inst->handle, &inst->async, &len, FALSE);
		ResetEvent(notify->event);
		ReadDirectoryChangesW(inst->handle, &inst->data, sizeof(inst->data), TRUE, FILE_NOTIFY_CHANGE_LAST_WRITE, NULL, &inst->async, 0);
		printf("Yay! [%ls]\n", inst->data.info.FileName);
	}

	/*
	unsigned int i = 0;
	struct sys_notify_inst_t *inst;
	for(inst = notify->inst; inst != NULL; inst = inst->next) {
		if(poll[i].revents & sys_poll_in_e) {
			FILE_NOTIFY_INFORMATION info;
			DWORD flags = FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_DIR_NAME;
			DWORD n;

			BOOL ret;

			printf("READ\n");
			ret = ReadDirectoryChangesW(inst->handle, &info, sizeof(info), FALSE, flags, &n, NULL, NULL);
			printf("HI %d\n", ret);

			return;
		}

		i++;
	}
	*/
}

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
 * Asynchronously poll the change notifier.
 *   @notify: The notifier.
 *   @func: The change function.
 *   @arg: The argument.
 *   &returns: The task.
 */
struct sys_task_t *sys_notify_async(struct sys_notify_t *notify, sys_change_f func, void *arg)
{
	struct notify_async_t *info;

	info = malloc(sizeof(struct notify_async_t));
	*info = (struct notify_async_t){ notify, func, arg };

	return sys_task_new(notify_task, info);
}

struct sys_task_t *sys_notify_async1(const char *path, sys_change_f func, void *arg)
{
	struct sys_notify_t *notify;

	notify = sys_notify_new();
	chkwarn(sys_notify_add(notify, path, NULL));

	return sys_notify_async(notify, func, arg);
}

/**
 * Notify task thread.
 *   @fd: The synchronization file descriptor.
 *   @arg: The argument.
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

		printf("wake\n");
		if(fds[0].revents)
			break;
		else if(fds[1].revents) {
			printf("HERE?\n");
			sys_notify_proc(info.notify, fds + 1);
			info.func(NULL, info.arg);
		}
	}

	sys_notify_delete(info.notify);
}
