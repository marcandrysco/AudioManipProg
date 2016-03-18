#ifndef THREAD_H
#define THREAD_H

/**
 * Thread structure.
 *   @lock: The critical section lock.
 */
struct sys_mutex_t {
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

//#define SYS_MUTEX_INIT PTHREAD_MUTEX_INIT

#endif
