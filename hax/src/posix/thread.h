#ifndef THREAD_H
#define THREAD_H

/*
 * common headers
 */

#include <pthread.h>

/**
 * Thread structure.
 *   @pthrd: The internal pthread mutex.
 */
struct sys_thread_t {
	pthread_t pthrd;
};

/**
 * Mutex structure.
 *   @pthrd: The internal pthread mutex.
 */
struct sys_mutex_t {
	pthread_mutex_t pthrd;
};

/*
 * thread declarations
 */
typedef struct sys_thread_t sys_thread_t;

sys_thread_t sys_thread_create(unsigned int flags, void *(*func)(void *), void *arg);
void *sys_thread_join(sys_thread_t *thread);
void sys_thread_detach(sys_thread_t *thread);

/*
 * mutex declarations
 */
typedef struct sys_mutex_t sys_mutex_t;

struct sys_mutex_t sys_mutex_init(unsigned int flags);
void sys_mutex_destroy(struct sys_mutex_t *mutex);

void sys_mutex_lock(struct sys_mutex_t *mutex);
bool sys_mutex_trylock(struct sys_mutex_t *mutex);
void sys_mutex_unlock(struct sys_mutex_t *mutex);

#define SYS_MUTEX_INIT PTHREAD_MUTEX_INITIALIZER


/**
 * Task function.
 *   @fd: The termiantion file descriptor.
 *   @arg: The argument.
 */
typedef void (*sys_task_f)(sys_fd_t fd, void *arg);

/*
 * task declarations
 */
struct sys_task_t *sys_task_new(sys_task_f func, void *arg);
void sys_task_delete(struct sys_task_t *task);

#endif
