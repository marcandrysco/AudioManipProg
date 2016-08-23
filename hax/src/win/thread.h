#ifndef THREAD_H
#define THREAD_H

/**
 * Thread structure.
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
 * task declarations
 */
struct sys_task_t;

struct sys_task_t *sys_task_new(void * ,void *);
void sys_task_delete(struct sys_task_t *task);

#endif
