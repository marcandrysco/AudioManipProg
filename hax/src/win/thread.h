#ifndef THREAD_H
#define THREAD_H

/**
 * Mutex structure.
 *   @once: One-time initializer.
 *   @lock: The critical section lock.
 */
struct sys_mutex_t {
	INIT_ONCE once;
	CRITICAL_SECTION lock;
};

/*
 * mutex declarations
 */
typedef struct sys_mutex_t sys_mutex_t;

sys_mutex_t sys_mutex_init(unsigned int flags);
void sys_mutex_destroy(sys_mutex_t *mutex);

void sys_mutex_lock(sys_mutex_t *mutex);
bool sys_mutex_trylock(sys_mutex_t *mutex);
void sys_mutex_unlock(sys_mutex_t *mutex);

#define SYS_MUTEX_INIT { INIT_ONCE_STATIC_INIT }

/*
 * condition variable declarations
 */
typedef CONDITION_VARIABLE sys_cond_t;

sys_cond_t sys_cond_init(unsigned int flags);
void sys_cond_destroy(sys_cond_t *cond);

void sys_cond_wait(sys_cond_t *cond, sys_mutex_t *mutex);
void sys_cond_signal(sys_cond_t *cond);
void sys_cond_broadcast(sys_cond_t *cond);


/**
 * Thread structure.
 *   @func: The function.
 *   @arg, ptr: The argument and return values.
 *   @handle: The handle
 *   @lock: The lock.
 *   @detach: Detach flag.
 */
struct sys_thread_t {
	void *(*func)(void *);
	void *arg, *ret;
	HANDLE handle;
	CRITICAL_SECTION lock;
	bool detach;
};

/*
 * thread declarations
 */
typedef struct sys_thread_t *sys_thread_t;

sys_thread_t sys_thread_create(unsigned int flags, void *(*func)(void *), void *arg);
void *sys_thread_join(sys_thread_t *thread);
void sys_thread_detach(sys_thread_t *thread);


/**
 * Task function.
 *   @fd: The termiantion file descriptor.
 *   @arg: The argument.
 */
typedef void (*sys_task_f)(sys_fd_t fd, void *arg);

/*
 * task declarations
 */
struct sys_task_t;

struct sys_task_t *sys_task_new(sys_task_f func, void *arg);
void sys_task_delete(struct sys_task_t *task);

#endif
