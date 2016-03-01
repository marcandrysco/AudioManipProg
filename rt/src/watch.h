#ifndef NOTIFY_H
#define NOTIFY_H

/**
 * Notify function.
 *   @path: The path.
 *   @arg: The argument.
 */

typedef void (*amp_notify_f)(const char *path, void *arg);


/*
 * notifier declarations
 */

struct amp_notify_t *amp_notify_new(char **list, amp_notify_f func, void *arg);
void amp_notify_delete(struct amp_notify_t *notify);

#endif
