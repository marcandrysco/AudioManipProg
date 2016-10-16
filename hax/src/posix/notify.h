#ifndef POSIX_NOTIFY_H
#define POSIX_NOTIFY_H

/**
 * Change notification structure.
 */
struct sys_change_t {
	char *name;
};

/*
 * notifier declarations
 */
struct sys_notify_t;

struct sys_notify_t *sys_notify_new(void);
void sys_notify_delete(struct sys_notify_t *notify);

char *sys_notify_add(struct sys_notify_t *notify, const char *path, int *id);

sys_fd_t sys_notify_fd(struct sys_notify_t *notify);
void sys_notify_proc(struct sys_notify_t *notify, struct sys_poll_t *poll);

#endif
