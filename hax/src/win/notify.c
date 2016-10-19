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
		uint8_t buf[sizeof(FILE_NOTIFY_INFORMATION) + 2 * MAX_PATH + 2];
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

	return NULL;
}


/**
 * Retrieve the descriptor for the change notifier.
 *   @notify: The notifier.
 *   &returns: The descriptor.
 */
sys_fd_t sys_notify_fd(struct sys_notify_t *notify)
{
	return (sys_fd_t){ notify->event, INVALID_SOCKET };
}

/**
 * Process a poll of the notifier.
 *   @notify: The notifier.
 *   @poll: The poll array.
 */
struct sys_change_t *sys_notify_proc(struct sys_notify_t *notify, struct sys_poll_t *poll)
{
	struct sys_notify_inst_t *inst;

	for(inst = notify->inst; inst != NULL; inst = inst->next) {
		DWORD len;

		GetOverlappedResult(inst->handle, &inst->async, &len, FALSE);
		inst->data.info.FileName[inst->data.info.FileNameLength/2] = L'\0';

		{
			char path[w32_win2unix(NULL, inst->data.info.FileName)+1];

			w32_win2unix(path, inst->data.info.FileName);
			d_printf("change [%.*s]\n", inst->data.info.FileNameLength/2, path);
		}

		ResetEvent(notify->event);
		ReadDirectoryChangesW(inst->handle, &inst->data, sizeof(inst->data), TRUE, FILE_NOTIFY_CHANGE_LAST_WRITE, NULL, &inst->async, 0);
	}

	return NULL;
}
