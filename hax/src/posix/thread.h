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

/**
 * Condition variable structure.
 *   @pthrd: The internal thread condition variable.
 */
struct sys_cond_t {
	pthread_cond_t pthrd;
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

#define SYS_MUTEX_INIT { PTHREAD_MUTEX_INITIALIZER }

/*
 * condition variable declarations
 */
typedef struct sys_cond_t sys_cond_t;

sys_cond_t sys_cond_init(unsigned int flags);
void sys_cond_destroy(sys_cond_t *cond);

void sys_cond_wait(sys_cond_t *cond, sys_mutex_t *mutex);
void sys_cond_signal(sys_cond_t *cond);
void sys_cond_broadcast(sys_cond_t *cond);

/*
 * one-time initialization
 */
void sys_once(sys_once_t *once, void (*func)(void));

#define SYS_ONCE_INIT PTHREAD_ONCE_INIT


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
