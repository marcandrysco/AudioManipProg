#ifndef THREAD_H
#define THREAD_H

/*
 * common headers
 */

#include <pthread.h>

/**
 * Mutex structure.
 *   @pthrd: The internal pthread mutex.
 */
struct sys_mutex_t {
	pthread_mutex_t pthrd;
};

typedef struct sys_mutex_t sys_mutex_t;

/*
 * mutex declarations
 */
struct sys_mutex_t sys_mutex_init(unsigned int flags);
void sys_mutex_destroy(struct sys_mutex_t *mutex);

void sys_mutex_lock(struct sys_mutex_t *mutex);
bool sys_mutex_trylock(struct sys_mutex_t *mutex);
void sys_mutex_unlock(struct sys_mutex_t *mutex);

#define SYS_MUTEX_INIT PTHREAD_MUTEX_INIT

#endif
