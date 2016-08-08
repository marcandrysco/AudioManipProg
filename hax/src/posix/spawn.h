#ifndef POSIX_SPAWN_H
#define POSIX_SPAWN_H

/*
 * process definitions
 */
typedef int sys_pid_t;

/*
 * spawn declarations
 */
char *sys_spawn(sys_pid_t *pid, const char *path, char *const *argv);
int sys_wait(sys_pid_t pid);

#endif
