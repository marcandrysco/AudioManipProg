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

sys_fd_t sys_notify_fd(struct sys_notify_t *notify);
unsigned int sys_notify_poll(struct sys_notify_t *notify, struct sys_poll_t *poll);
struct sys_change_t *sys_notify_proc(struct sys_notify_t *notify, struct sys_poll_t *poll);

#endif
