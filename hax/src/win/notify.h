#ifndef NOTIFY_H
#define NOTIFY_H

/**
 * Change notification structure.
 */
struct sys_change_t {
	char *name;
};

/*
 * change notifier declarations
 */
struct sys_notify_t *sys_notify_new(void);
void sys_notify_delete(struct sys_notify_t *notify);

char *sys_notify_add(struct sys_notify_t *notify, const char *path, int *id);

unsigned int sys_notify_poll(struct sys_notify_t *notify, struct sys_poll_t *poll);
struct sys_change_t *sys_notify_proc(struct sys_notify_t *notify, struct sys_poll_t *poll);

/*
 * change notifier declarations
 */
struct sys_task_t *sys_notify_async(struct sys_notify_t *notify, sys_change_f func, void *arg);
struct sys_task_t *sys_notify_async1(const char *path, sys_change_f func, void *arg);

#endif
