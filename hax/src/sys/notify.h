#ifndef SYS_NOTIFY_H
#define SYS_NOITFY_H

/**
 * Change notification function.
 *   @chnage: The change structure.
 *   @arg: The argument.
 */
typedef void (*sys_change_f)(struct sys_change_t *change, void *arg);


/*
 * notify declarations
 */
struct sys_task_t *sys_notify_async(struct sys_notify_t *notify, sys_change_f func, void *arg);
struct sys_task_t *sys_notify_async1(const char *path, sys_change_f func, void *arg);

#endif
